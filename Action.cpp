//
// Created by jwc on 12/9/18.
//


#include <zconf.h>
#include "Action.h"
#include "file_exists.h"

using json = nlohmann::json;

Action::Action() {
    std::cout << "Action::Action()" << std::endl;

    actionFunctions["generate"] = &Action::generate;
    actionFunctions["drop"] = &Action::drop;
    actionFunctions["execute"] = &Action::execute;

    env = new Environment(envRoot);

    env->add_callback("map", 2, [this](Parsed::Arguments args, json x) {
        std::string map = env->get_argument<std::string>(args, 0, x);
        std::string key = env->get_argument<std::string>(args, 1, x);

        std::string r = key;
        try {
            r = x.at("map").at(map).at(key).get<std::string>();
        }
        catch (...) { ; // do nothing
        }
        return r;
    });

    env->add_callback("lCamel", 1, [this](Parsed::Arguments args, json x) {
        return snakeToCamel(env->get_argument<std::string>(args, 0, x));
    });

    env->add_callback("uCamel", 1, [this](Parsed::Arguments args, json x) {
        return snakeToCamel(env->get_argument<std::string>(args, 0, x), true);
    });

    env->add_callback("regex_search", 2, [this](Parsed::Arguments args, json x) {
        std::string value = env->get_argument<std::string>(args, 0, x);
        std::regex re(env->get_argument<std::string>(args, 1, x));

        return std::regex_search(value, re);
    });

    env->add_callback("regex_replace", 3, [this](Parsed::Arguments args, json x) {
        std::string subject = env->get_argument<std::string>(args, 0, x);
        std::regex regex(env->get_argument<std::string>(args, 1, x));
        std::string replacement = env->get_argument<std::string>(args, 2, x);

        return std::regex_replace(subject, regex, replacement);
    });
}

void Action::runAction(nlohmann::json target, nlohmann::json patch, nlohmann::json template_it) {
    std::cout << "Action::runAction()" << std::endl;

    for (auto action = template_it.begin(); action != template_it.end(); ++action) {
        std::cout << "action: " << action->dump(4) << std::endl;

        std::string actionName = action->at("action");
        std::cout << "actionName: " << actionName << std::endl;

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


bool Action::generate(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::generate()" << std::endl;
//    std::cout << "action: " << action.dump(4) << std::endl;

    std::string templateFileName = action.at("inga");
    bool overwrite = action.at("overwrite");
    nlohmann::json data = (action.at("data") == "target") ? target : patch;

    std::string outputTemplate = action.at("out");
    std::string outputFileName = env->render(outputTemplate, data);

    data["map"] = nlohmann::json({});  // TODO: FIX THIS

    if (file_exists(envRoot + outputFileName)) {
        if (!overwrite) {
            std::cout << "Not overwriting: " << envRoot + outputFileName << std::endl;
            return true;
        }
        else {
            std::cout << "Overwriting: " << envRoot + outputFileName << std::endl;
        }
    }
    else {
        std::cout << "Generating: " << envRoot + outputFileName << std::endl;
    }

//    std::cout << "templateFileName:" << templateFileName << std::endl;
//    std::cout << "data:" << data << std::endl;

    Template temp = env->parse_template(templateFileName);
    env->write(temp, data, outputFileName);

    return true;
}

bool Action::drop(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::drop()" << std::endl;
    std::cout << "action: " << action.dump(4) << std::endl;

    return true;
}

bool Action::execute(const nlohmann::json &target, const nlohmann::json &patch, const nlohmann::json &action) {
    std::cout << "Action::execute()" << std::endl;
    std::cout << "action: " << action.dump(4) << std::endl;

    return true;
}