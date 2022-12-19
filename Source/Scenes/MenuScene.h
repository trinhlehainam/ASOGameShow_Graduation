#pragma once
#include "IScene.h"

#include <functional>
#include <vector>

#include "../Math/vec2.h"

class MenuScene: public IScene
{
public:
    MenuScene();
    ~MenuScene();

private:
    bool Init() override;
    void Update(float deltaTime_s) override;
    void RenderToOwnScreen() override;
    std::unique_ptr<IScene> ChangeScene(std::unique_ptr<IScene> scene) override;
    SCENE_ID GetSceneID() override;
    struct MenuItem
    {
        std::string menuText;
        vec2f pos;
        std::function<void(void)> func;
        bool isActive = false;
        MenuItem(const char* str, const vec2f& p, std::function<void(void)> f) :
            menuText(str),
            pos(p),
            func(f) {};
    };
    std::vector<MenuItem> menuItems_;

    using DrawFunc_t = void (MenuScene::*)();
    using UpdateFunc_t = void (MenuScene::*)(const float&);

    // Pointer function for draw method
    DrawFunc_t drawFunc_;
    // Pointer function for update method
    UpdateFunc_t updateFunc_;

    // Draw after MenuScene constructed
    void AppearDraw();
    // Draw before MenuScene constructed
    void DisappearDraw();
    // Draw after AppearDraw done
    void NormalDraw();

    // Update information to AppearDraw()
    void AppearUpdate(const float&);
    // Update information to DisappearDraw()
    void DisappearUpdate(const float&);
    // Update information to MenuScene at normar state
    void NormalUpdate(const float&);

    void SetCurrentItem();

    void CloseScene();
};

