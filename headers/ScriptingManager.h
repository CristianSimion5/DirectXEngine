#ifndef _SCRIPTING_MANAGER_H_
#define _SCRIPTING_MANAGER_H_

#include <lua/lua.hpp>
#include <sol/sol.hpp>

#include "Transform.h"

class ScriptingManager {
public:
    ScriptingManager() {
        lua = sol::state{};
        lua.open_libraries(sol::lib::base, sol::lib::io);

        lua.new_usertype<Vector3>("vector3",
            sol::constructors<
                void(),
                void(float x),
                void(float x, float y, float z)
            >(),
            "x", &Vector3::x,
            "y", &Vector3::y,
            "z", &Vector3::z,
            "dot", &Vector3::Dot);

        //lua.new_usertype<Transform>("transform", 
        //    sol::constructors<
        //        void(const Matrix& parentGlobalMatrix, Vector3 position, Vector3 rotation, Vector3 scale),
        //        void(Vector3 position, Vector3 rotation, Vector3 scale)
        //    >(),
        //    "pos", &Transform::position,
        //    "rot", &Transform::rotation,
        //    "scl", &Transform::scale);
    
        //lua.script_file("scripts/script.lua");

        //const std::function<void()>& luaNakedFunc = lua["naked_func"];
        //luaNakedFunc();
        //const std::function<void()>& closeFunc = lua["close_io"];
        
        //const std::function<void(float)>& luaWrite = lua["write"];

        //const std::function<float(Vector3, Vector3)>& luaDot = lua["dot"];
        //luaWrite(luaDot(Vector3(1, 2, 3), Vector3::One * 2.f));

        //closeFunc();

        lua.script_file("scripts/move_cycle.lua");
        luaUpdate = lua["update"];
    }

public:
    sol::state lua{};
    std::function<float(Vector3& pos, float& dirZ, float deltaTime)> luaUpdate;
};

#endif // !_SCRIPTING_MANAGER_H_