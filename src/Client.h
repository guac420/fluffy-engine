#ifndef GAMEFRAME_FRSTPERSONPLAYER_H
#define GAMEFRAME_FRSTPERSONPLAYER_H

#include "gfx/Window.h"
#include "Player.h"
#include "Camera.h"
#include "Ui.h"
#include "World.h"
#include "utils/Utils.h"

// https://github.com/222464/EvolvedVirtualCreaturesRepo/blob/master/VirtualCreatures/Volumetric_SDL/Source/SceneObjects/Physics/DynamicCharacterController.cpp
// Csgo Movement: https://adrianb.io/2015/02/14/bunnyhop.html
// Eventually want to write custom friction management
// using ghost object to apply friction from filtered contact points
class Client : public Player, public Camera {
private:
    Window* window;
    World* world;
    const float accelerationConstant = 5.f, jumpImpulse = 4.f;
    bool onGround = false;
    int jumpDelay = 100;
    unsigned long long lastJump = 0;

    // Ui
    bool show = true;
    float friction;

    class IgnoreBodyCast :
            public btCollisionWorld::ClosestRayResultCallback
    {
    private:
        btRigidBody* pBody;

    public:
        explicit IgnoreBodyCast(btRigidBody* pBody)
                : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
                  pBody(pBody)
        {
        }

        btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override {
            if(rayResult.m_collisionObject == pBody)
                return 1.0f;

            return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }
    };

    /**
     * Calculates velocity based on user input
     * @param acceleration where to store acceleration
     * @return Whether or not force should be applied to the body
     */
    bool getAcceleration(btVector3& acceleration) {
#define IS_DOWN(x) (this->window->keyboardManager.isKeyDown(x))
        btVector3 forward = btVector3(Camera::front.x, 0, Camera::front.z),
                right = btVector3(Camera::right.x, 0, Camera::right.z);

        acceleration =
                ((float)IS_DOWN(GLFW_KEY_W) * forward) -
                ((float)IS_DOWN(GLFW_KEY_S) * forward) +
                ((float)IS_DOWN(GLFW_KEY_A) * right) -
                ((float)IS_DOWN(GLFW_KEY_D) * right);
        return !acceleration.isZero();
    }

    void groundTest() {
        const float testOffset = 0.07f;
        IgnoreBodyCast callback(this->rigidBody);
        world->getWorld()->rayTest(this->transform.getOrigin(),
                                   this->transform.getOrigin()-btVector3(0.f, this->verticalOffset + testOffset, 0.f),
                                   callback);
        this->onGround = callback.hasHit();
    }

public:
    explicit Client(Window* window, World* world) :
            window(window), world(world) {
        // Setup physics
        Player::initializeBody();

        // Set default start position
        Thing::setPosition(glm::vec3(0.f, 1.f, 0.f));
        Camera::updateView(glm::vec3(0.f, 1.f, 0.f));

        // Create projection matrix
        Camera::resize(window->width, window->height);

        // Don't render client object
        this->rigidBody->setCollisionFlags(btRigidBody::CollisionFlags::CF_DISABLE_VISUALIZE_OBJECT);
        this->friction = this->rigidBody->getFriction();
    }

    /**
     * Update camera look-vectors and acceleration
     */
    void updateFrame() {
        if (!window->cursorLocked) return;   // Don't update if in Ui
        Camera::updateFrame(this->window);

        this->groundTest();

        btVector3 acceleration;
        if (this->getAcceleration(acceleration)) {
            acceleration = this->accelerationConstant * acceleration.normalize();
            this->rigidBody->activate();
            this->rigidBody->applyCentralForce(acceleration);
        }
        // Check if on ground, if key pressed, and how long since last jump
        if (onGround && (IS_DOWN(GLFW_KEY_SPACE) * NOW() / NS_PER_MS) > lastJump + jumpDelay) {
            lastJump = NOW() / NS_PER_MS;
            this->rigidBody->activate();
            this->rigidBody->applyCentralImpulse(btVector3(0.f, this->jumpImpulse, 0.f));
        }
    }

    void updateGui() {
        if (!Ui::isActive()) return; // Don't push ui if not active
        if (ImGui::Begin("Client", &this->show)) {
            if (ImGui::SliderFloat("Friction", &this->friction, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp)) {
                this->rigidBody->setFriction(this->friction);
            }
        }
        ImGui::End();
    }

    /**
     * Update the view matrix.
     * Call after world is updated
     */
    void updateTransform() override {
        Thing::updateTransform();
        Camera::updateView(this->getPosition());
    }

    /**
     * Set rigid body position and update view matrix
     * @param position the position to move to
     */
    void setPosition(const glm::vec3& position) override {
        Thing::setPosition(position);
        Camera::updateView(position);
    }

    void resize() {
        Camera::resize(window->width, window->height);
    }
};

#undef IS_DOWN

#endif //GAMEFRAME_FRSTPERSONPLAYER_H
