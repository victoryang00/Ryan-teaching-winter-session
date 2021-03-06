//
// Created by yiwei yang on 2/5/22.
//

#include "EatKanoPanel.h"
#include "GameOverPanel.h"
#include "engine/Options.h"
#include "engine/Screen.h"

#include <boost/lexical_cast.hpp>
#include <iomanip>
#include <thread>

bool clicked_wrong = false;

EatKanoPanel::EatKanoPanel(const Mode type) : type_(type), score(0) {

    int j = 0;
    for (auto &&i : getOptions().inputKey) {
        keys_[j] = i;
        j++;
    }
    for (int &i : curr_panel) {
        auto rand_int = calRand();
        i = rand_int;
    }
    for (int i = 0; i < 3; i++) {
        this->keys_[i] = getOptions().inputKey[i];
    }
    j = 0;

    for (auto &inner_map_ : map_)
        for (auto &inner_map_2 : inner_map_) {
            check_box_.emplace_back(std::make_shared<HiddenButton>(HiddenButton(
                [this] { UpdatePanel(); }, keys_[j % 4], std::get<0>(inner_map_2), std::get<1>(inner_map_2))));
            j++;
        }
    for (int i = 0; i < 3; i++) {
        this->check_box_[20 + i]->setDown();
    }
    /** Draw the hidden box */
    for (auto &i : check_box_)
        addWidget(i);
    j = 0;
    for (auto &i : curr_panel) {
        auto senpai_ = std::make_shared<SenPai>(SenPai([this] { UpdatePanel(); }, keys_[j % 4], i));
        widgets_.emplace_back(senpai_);
        addWidget(senpai_);
        senpai_->setCenter(std::get<0>(map_[j][i]), std::get<1>(map_[j][i]));
        j++;
    }
    this->widgets_[5]->setDown();

    /** Update correctness */
    this->check_box_[20 + curr_panel[5]]->setCorrect();
}

void EatKanoPanel::draw() const {
    GetScreen().DrawCentered("board", 0, 0);

    if (getType() == Mode::NORMAL) {
        DrawTime(-200, -410);
    } else if (getType() == Mode::ENDLESS) {
        DrawCPS(-200, -410);
    }
    DrawWidgets();
}
void EatKanoPanel::step() {
    StepWidgets();
    auto time_ = getTime();
    /** To determine the game over or not */
    if (gameOver()) {
        /** Show the music and blink */
        jngl::play("./music/end.ogg");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        jngl::play("./music/err.ogg");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        if (getType() == Mode::NORMAL) {
            jngl::setWork(std::make_shared<GameOverPanel>(type_, Data(score, time_), getOptions().normalHighscore_));
        } else if (getType() == Mode::ENDLESS) {
            jngl::setWork(
                std::make_shared<GameOverPanel>(type_, Data(calCPS(), time_), getOptions().endlessHighscore_));
        }
    } else {
        int j = 0;
        for (auto &i : curr_panel) {
            widgets_[j]->setCenter(std::get<0>(map_[j][i]), std::get<1>(map_[j][i]));

            j++;
        }
        DrawWidgets();
    }

    if (jngl::keyPressed('p') || jngl::keyPressed(jngl::key::Escape)) {
        onQuitEvent(); // Pause
    }
}

void EatKanoPanel::onQuitEvent() {
    if (20 - jngl::getTime() > 0.0001) { // Don't allow pausing the game more then one time per second
        jngl::cancelQuit();
    }
}

void EatKanoPanel::DrawTime(const int x, const int y) const {
    jngl::setFontSize(60);
    jngl::setFontColor(0, 0, 0);
    jngl::print("Time: ", x, y);
    auto time = getTime();
    int minutes = int(time / 60);
    int seconds = int(time - minutes * 60);
    if (started_) {
        std::stringstream sstream;
        sstream.fill('0');
        sstream << 20 - seconds;
        jngl::print(sstream.str(), x + 220, y);
    } else {
        jngl::print("20", x + 220, y);
    }
}

long long EatKanoPanel::getTime() const {
    if (started_)
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time_).count();
    else
        return 0;
}

EatKanoPanel::Mode EatKanoPanel::getType() const { return type_; }

bool EatKanoPanel::gameOver() const {
    if (20 - getTime() < 1 || clicked_wrong) {
        return true;
    }
    return false;
}

void EatKanoPanel::DrawCPS(const int x, const int y) const {
    jngl::setFontSize(60);
    jngl::setFontColor(255, 255, 255);
    jngl::print("CPS: ", x, y);
    std::stringstream sstream;
    sstream.fill('0');
    sstream << calCPS();
    sstream << std::fixed << std::setprecision(2) << calCPS();
    jngl::print(sstream.str(), x + 220, y);
}

float EatKanoPanel::calCPS() const { return score / getTime(); }

void EatKanoPanel::UpdatePanel() {
    /** make the updated button clicked and check one by one, if not success blink, if success rerand */
    if (clicked_wrong) {
        return;
    }
    /** Update rand */
    int new_curr_panel[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 1; i < 6; i++) {
        new_curr_panel[i] = curr_panel[i - 1];
    }
    new_curr_panel[0] = calRand();
    std::memcpy(curr_panel, new_curr_panel, sizeof(new_curr_panel));
    /** Update down */
    this->widgets_[5]->setDown();

    /** Update correctness */
    for (int i = 0; i < 4; i++) {
        check_box_[i + 20]->setIncorrect();
    }
    this->check_box_[20 + curr_panel[5]]->setCorrect();

    for (int i = 0; i < 3; i++) {
        this->check_box_[20 + i]->setDown();
    }
    if (!started_) {
        started_ = true;
    }
    jngl::play("./music/tap.ogg");
}
