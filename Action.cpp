//
// Created by jwc on 12/9/18.
//


#include <zconf.h>
#include "Action.h"

using json = nlohmann::json;

Action::Action() {
    std::cout << "Action::Action()" << std::endl;

    actionFunctions["generate"] = &Action::generate;
    actionFunctions["drop"] = &Action::drop;
    actionFunctions["execute"] = &Action::execute;

    env = new Environment("../");

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


//    YAML::Node yamlSchema = YAML::LoadFile("../migration01.yaml");
//    assert(yamlSchema.IsDefined()); // TODO: add proper error handling
//    assert(yamlSchema.IsMap()); // TODO: add proper error handling
//    nlohmann::json schema = YAMLtoJSON(yamlSchema);
//
//    YAML::Node yamlMap = YAML::LoadFile("../map.yaml");
//    assert(yamlMap.IsDefined()); // TODO: add proper error handling
//    assert(yamlMap.IsMap()); // TODO: add proper error handling
//    schema["map"] = YAMLtoJSON(yamlMap);
//
//    std::cout << "schema:" << schema.dump(4) << std::endl;
//
//
//    std::string result1 = env.render_file("table_create.inja", schema);
//    std::string result2 = env.render_file("extjs_model_create.inja", schema);
//    std::string result3 = env.render_file("tf_sqlobject.inja", schema);
//    std::string result4 = env.render_file("tf_model_h.inja", schema);
//    std::string result5 = env.render_file("tf_model_cpp.inja", schema);
//
//    std::cout << "SQL:" << result1 << std::endl;
//    std::cout << "ExtJS:" << result2 << std::endl;
//    std::cout << "tf_sqlobject:" << result3 << std::endl;
//    std::cout << "tf_model_h:" << result4 << std::endl;
//    std::cout << "tf_model_cpp:" << result5 << std::endl;

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
    std::cout << "action: " << action.dump(4) << std::endl;

    std::string templateFileName = action.at("inga");
    std::string outputFileName = action.at("out");
    bool overwrite = action.at("overwrite");
    nlohmann::json data = (action.at("data") == "target") ? target : patch;

    data["map"] = nlohmann::json({});  // TODO: FIX THIS


    std::cout << "templateFileName:" << templateFileName << std::endl;
    std::cout << "data:" << data << std::endl;
    std::string result = env->render_file(templateFileName, data);
    std::cout << "Result:" << result << std::endl;
    exit(1);
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