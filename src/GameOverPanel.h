//
// Created by yiwei yang on 2/13/22.
//

#ifndef RYAN_TEACHING_WINTER_SESSION_GAMEOVERPANEL_H
#define RYAN_TEACHING_WINTER_SESSION_GAMEOVERPANEL_H

#include "ButtonBox.h"
#include "Scores.h"
#include "engine/Work.h"
#include <jngl/Vec2.hpp>

class EatKanoPanel;

class GameOverPanel : public Work {
public:
    GameOverPanel(EatKanoPanel::Mode mode_, const Data &, HighScore);
    void step() override;
    void draw() const override;
    void onQuitEvent() override;
    void OnBack(EatKanoPanel::Mode mode_) const;
    void BlinkHighscore(Data data);
private:
    Data data_;
    HighScore highscore_;
    std::shared_ptr<ButtonBox> restart_;
};

#endif // RYAN_TEACHING_WINTER_SESSION_GAMEOVERPANEL_H
