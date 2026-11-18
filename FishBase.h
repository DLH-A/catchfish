#pragma once
#include "../core/Component.h"
#include "ResourceManager.h"
#include <raylib.h>
#include <string>

class FishBase : public Component {
protected:
    Vector2 position_;
    Vector2 direction_;
    float speed_;
    float radius_;
    Color color_;
    bool hasEnteredScreen_;


    // 贴图相关
    Texture2D texture_;
    std::string texturePath_;
    bool textureLoaded_ = false;

    // ===== 新增：动画相关变量 =====
    int   frameCount_ = 1;      // 总帧数（1=单帧，>1=动画）
    float frameWidth_ = 0;      // 每帧宽度
    float frameHeight_ = 0;      // 每帧高度
    int   currentFrame_ = 0;      // 当前播放的帧索引
    float frameTimer_ = 0.0f;   // 帧计时器
    float frameDuration_ = 0.1f;   // 每帧持续时间（秒）
    bool  isAnimated_ = false;  // 是否是动画

public:
    int hp_ = 1;                                 // 👈 新加的三行
    bool isDead_ = false;                        // 👈 
    Rectangle hitbox_ = { 0, 0, 60.0f, 40.0f };  // 👈
    int score = 10; // 【新增】这条鱼被消灭后获得的分数
    // 【新增】手动初始化方法
    void ManualInit(Game* g) {
        game = g;       // 设置游戏环境指针
        Init();         // 手动调用 Init，这会执行加载贴图的代码
    }

    // 【新增】强制初始化并加载贴图
    void ForceInit(Game* g) {
        game = g; // 设置环境

        // 直接尝试加载图片
        if (!texturePath_.empty()) {
            texture_ = LoadTexture(texturePath_.c_str());
            if (texture_.id != 0) {
                textureLoaded_ = true;
                // 如果半径太小，自动根据图片大小调整
                if (radius_ < 10.0f) {
                    radius_ = (float)texture_.width / 2.0f;
                }
                // 新增：如果是动画且没指定宽高，自动按垂直排列计算
                if (isAnimated_ && frameWidth_ == 0) {
                    frameWidth_ = (float)texture_.width;
                    frameHeight_ = (float)texture_.height / frameCount_;
                }
            }
            else {
                // 加载失败，打印错误方便调试
                TraceLog(LOG_WARNING, "Failed to load fish texture: %s", texturePath_.c_str());
            }
        }
    }

    FishBase() : speed_(0), radius_(0), hasEnteredScreen_(false) {
        position_ = { 0, 0 };
        direction_ = { 0, 0 };
        color_ = WHITE;
        texture_ = { 0 };
    }

    // 使用贴图初始化
       // 正确的写法：
    void SetupWithTexture(Vector2 startPos, Vector2 targetPos, float speed, float radius, Color color, const std::string& texturePath,
        int frameCount = 1, float frameWidth = 0, float frameHeight = 0, float frameDuration = 0.1f) {
        Setup(startPos, targetPos, speed, radius, color);
        texturePath_ = texturePath;
        frameCount_ = frameCount;
        frameWidth_ = frameWidth;
        frameHeight_ = frameHeight;
        frameDuration_ = frameDuration;
        isAnimated_ = (frameCount > 1); // 帧数大于1才算动画
    }

    // 原有的 Setup 方法（保留用于纯色模式）
    void Setup(Vector2 startPos, Vector2 targetPos, float speed, float radius, Color color) {
        position_ = startPos;

        Vector2 diff = { targetPos.x - startPos.x, targetPos.y - startPos.y };
        float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
        if (length > 0) {
            direction_ = { diff.x / length, diff.y / length };
        }
        else {
            direction_ = { 1.0f, 0.0f };
        }

        speed_ = speed;
        radius_ = radius;
        color_ = color;
        hasEnteredScreen_ = false;
        hp_ = 1;
    }

    void Init() override {
        // 加载贴图
        if (!texturePath_.empty() && game != nullptr) {
            ImageHandle imgHandle = game->resourceManager->GetImage(texturePath_);
            if (imgHandle && !imgHandle->texture.id) {
                // 如果纹理未加载，尝试重新加载
                texture_ = LoadTexture(texturePath_.c_str());
                if (texture_.id != 0) {
                    textureLoaded_ = true;
                    // 根据贴图大小调整半径
                    if (radius_ <= 0) {
                        radius_ = (float)texture_.width / 2.0f;
                    }
                }
            }
            else if (imgHandle) {
                texture_ = imgHandle->texture;
                textureLoaded_ = true;
                if (radius_ <= 0) {
                    radius_ = (float)texture_.width / 2.0f;
                }
            }
        }
    }

    void update(float dt) override {
        if (!isActive) return;

        position_.x += direction_.x * speed_ * dt;
        position_.y += direction_.y * speed_ * dt;

        // 新增：动画帧更新
        if (isAnimated_ && textureLoaded_) {
            frameTimer_ += dt;
            if (frameTimer_ >= frameDuration_) {
                frameTimer_ = 0.0f;
                currentFrame_ = (currentFrame_ + 1) % frameCount_; // 循环播放
            }
        }

        // 假设碰撞箱大小是 60x40（你可以根据鱼的图片大小调整）
        hitbox_.x = position_.x - 30.0f;
        hitbox_.y = position_.y - 20.0f;
        hitbox_.width = 60.0f;
        hitbox_.height = 40.0f;

        if (!hasEnteredScreen_) {
            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();

            if (position_.x >= radius_ &&
                position_.x <= screenWidth - radius_ &&
                position_.y >= radius_ &&
                position_.y <= screenHeight - radius_) {
                hasEnteredScreen_ = true;
            }
        }
    }

    void draw() override {
        if (!isActive) return;

        if (textureLoaded_ && texture_.id != 0) {
            // 计算旋转角度（弧度转角度）
            float angle = atan2f(direction_.y, direction_.x) * 180.0f / PI;

            // 绘制旋转的贴图
            // 注意：DrawTexturePro 需要调整源矩形和目标矩形
                       // 新增：根据是否动画，决定裁剪哪一帧
            Rectangle source;
            float fw, fh; // 当前帧的实际宽高
            if (isAnimated_) {
                fw = frameWidth_;
                fh = frameHeight_;
                source = { 0, (float)currentFrame_ * fh, fw, fh }; // 垂直裁剪当前帧
            }
            else {
                fw = (float)texture_.width;
                fh = (float)texture_.height;
                source = { 0, 0, fw, fh };
            }

            // 按原图宽高比计算显示尺寸，防止鱼被拉伸变形
            float dispW = radius_ * 3.0f; // 你原来的放大倍数
            float dispH = dispW * (fh / fw);
            Rectangle dest = { position_.x - dispW / 2.0f, position_.y - dispH / 2.0f, dispW, dispH };
            Vector2 origin = { dispW / 2.0f, dispH / 2.0f }; // 绕中心旋转

            DrawTexturePro(texture_, source, dest, origin, angle, WHITE);
        }
        else {
            // 降级方案：绘制圆形
            DrawCircleV(position_, radius_, color_);

            // 绘制眼睛表示朝向
            float eyeOffset = radius_ * 0.5f;
            Vector2 eyePos = {
                position_.x + direction_.x * eyeOffset,
                position_.y + direction_.y * eyeOffset
            };
            DrawCircleV(eyePos, radius_ * 0.3f, WHITE);
        }
    }

    void destroy() override {
        // 如果是我们自己加载的纹理，需要卸载
        if (textureLoaded_ && texture_.id != 0 && texturePath_.empty()) {
            UnloadTexture(texture_);
            texture_ = { 0 };
        }
    }

    bool IsReadyToRemove() const {
        if (!hasEnteredScreen_) return false;

        float screenWidth = GetScreenWidth();
        float screenHeight = GetScreenHeight();
        float buffer = radius_ + 10.0f;

        return (position_.x < -buffer ||
            position_.x > screenWidth + buffer ||
            position_.y < -buffer ||
            position_.y > screenHeight + buffer);
    }

    float GetRadius() const { return radius_; }
};