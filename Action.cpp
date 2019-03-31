//
// Created by jwc on 12/9/18.
//


#include <zconf.h>
#include <sys/wait.h>
#include "Action.h"
#include "file_exists.h"
#include "split_string.h"

Action::Action() { // TODO: add "map" to constructor
    std::cout << "Action::Action()" << std::endl;

    actionFunctions["generate"] = &Action::generate;
    actionFunctions["drop"] = &Action::drop;
    actionFunctions["execute"] = &Action::execute;

    env = new Environment(envRoot);

    env->add_callback("setValue", 1, [this](Arguments &args) {
        //std::string text = env->get_argument<std::string>(args, 0, x);
        std::string text = args.at(0)->get<std::string>();

        std::vector<std::string> v = split(text, "|");

        this->values[v[0]] = (v.size() == 2) ? v[1] : "";
        return "";
    });

    env->add_callback("getValue", 1, [this](Arguments &args) {
        std::string key = args.at(0)->get<std::string>();

        auto value = this->values.find(key);
        return (value != this->values.end()) ? value->second : "";
    });

    env->add_callback("map", 2, [this](Arguments &args) {
        std::string mapArg = args.at(0)->get<std::string>();
        std::string keyArg = args.at(1)->get<std::string>();

        std::string r = "*** map: " + mapArg + " key not found:" + keyArg + " ***";
        try {
            r = this->map.at(mapArg).at(keyArg).get<std::string>();
        }
        catch (nlohmann::json::out_of_range &e) { ;
            // do nothing
        }
        return r;
    });

    env->add_callback("mapBool", 2, [this](Arguments &args) {
        std::string mapArg = args.at(0)->get<std::string>();
        std::string keyArg = args.at(1)->get<std::string>();

        bool r = false;
        try {
            r = this->map.at(mapArg).at(keyArg).get<bool>();
        }
        catch (nlohmann::json::out_of_range &e) { ;
            // do nothing
        }
        return r;
    });

    env->add_callback("render", 1, [this](Arguments &args) {
        std::string arg = args.at(0)->get<std::string>();
        json j;
        return this->env->render(arg, j);
    });

    env->add_callback("lCamel", 1, [this](Arguments &args) {
        return snakeToCamel(args.at(0)->get<std::string>());
    });

    env->add_callback("uCamel", 1, [this](Arguments &args) {
        return snakeToCamel(args.at(0)->get<std::string>(), true);
    });

    env->add_callback("uSpace", 1, [this](Arguments &args) {
        std::string s = args.at(0)->get<std::string>();
        std::string r;

        bool underscore = false;
        for (int i = 0; i < s.length(); ++i) {

            char c = s[i];

            if (i == 0)
                c = toupper(c);

            if (underscore)
                c = toupper(c);

            underscore = false;

            if (c == '_') {
                underscore = true;
                c = ' ';
            }

            r += c;
        }
        return r;
    });

    env->add_callback("regex_search", 2, [this](Arguments &args) {
        std::string value = args.at(0)->get<std::string>();
        std::regex re(args.at(1)->get<std::string>());

        return std::regex_search(value, re);
    });

    env->add_callback("regex_replace", 3, [this](Arguments &args) {
        std::string subject = args.at(0)->get<std::string>();
        std::regex regex(args.at(1)->get<std::string>());
        std::string replacement = args.at(2)->get<std::string>();

        return std::regex_replace(subject, regex, replacement);
    });
}

void Action::doAction(nlohmann::json target, nlohmann::json patch, nlohmann::json template_it) {
    std::cout << "Action::doAction()" << std::endl;

    for (auto action = template_it.begin(); action != template_it.end(); ++action) {

        std::string actionName = action->at("action");

        bool b = (this->*actionFunctions[actionName])(target, patch, *action);
    }
}

std::string Action::snakeToCamel(const std::string &snake, const bool initCap) {
    std::string r;

    bool underscore = false;
    for (int i = 0; i < snake.length(); ++i) {

        char c = snake[i];

        if (i == 0 && initCap)
            c = toupper(c);

        if (underscore)
            c = toupper(c);

        underscore = false;

        if (c == '_')
            underscore = true;
        else
            r += c;
    }
    return r;
}

void Action::setMap(const nlohmann::json &map) {
    this->map = map;
}

bool Action::generate(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::generate()" << std::endl;

    std::string templateFileName = action.at("inga");
    bool overwrite = action.at("overwrite");
    nlohmann::json data = (action.at("data") == "target") ? target : patch;
    std::cerr << data.dump(4) << std::endl;

    std::string outputTemplate = action.at("out");
    std::string outputFileName = env->render(outputTemplate, data);
    //data["map"] = this->map;

    if (file_exists(envRoot + outputFileName)) {
        if (!overwrite) {
            std::cerr << "Not overwriting: " << envRoot + outputFileName << std::endl;
            return true;
        }
        else {
            std::cerr << "Overwriting: " << envRoot + outputFileName << std::endl;
        }
    }
    else {
        std::cerr << "Generating: " << envRoot + outputFileName << std::endl;
    }

    Template temp = env->parse_template(templateFileName);
    env->write(temp, data, outputFileName);

    return true;
}

bool Action::drop(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::drop()" << std::endl;

    return true;
}

bool Action::execute(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::execute()" << std::endl;

    std::vector<char *> argVector;
    std::vector<char *> envVector;


    nlohmann::json data;

    try {
        data = (action.at("data") == "target") ? target : patch;
    }
    catch (nlohmann::json::out_of_range &e) {
        ; // do nothing
    }

    std::string run = action.at("run");
    argVector.push_back((char *) run.c_str());

    try {
        nlohmann::json args = action.at("args");
        for (auto arg_it = args.begin(); arg_it != args.end(); ++arg_it) {
            std::string *s = new std::string(env->render(*arg_it, data));
            argVector.push_back((char *) s->c_str());
        }
    }
    catch (nlohmann::json::out_of_range &e) {
        ; // do nothing
    }
    argVector.push_back(nullptr);

    //
    try {
        nlohmann::json envs = action.at("env");
        for (auto env_it = envs.begin(); env_it != envs.end(); ++env_it) {
            std::string *s = new std::string(env->render(*env_it, data));
            envVector.push_back((char *) s->c_str());
        }
    }
    catch (nlohmann::json::out_of_range &e) {
        ; // do nothing
    }
    envVector.push_back(nullptr);

    int status;
    int pid = fork();

    if (pid == 0) {
        execve(run.c_str(), argVector.data(), envVector.data());
    }

    std::cout << "Waiting for execute to complete..." << std::endl;

    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "Child returned error." << std::endl;
        return false;
    }

    return true;
}