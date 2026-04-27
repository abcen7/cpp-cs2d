#pragma once

#include "model/bonus.h"

class ArmorPack : public Bonus {
public:
    explicit ArmorPack(float positionX, float positionY);
    ~ArmorPack() override;

    ArmorPack(const ArmorPack&) = delete;
    ArmorPack& operator=(const ArmorPack&) = delete;
    ArmorPack(ArmorPack&&) = delete;
    ArmorPack& operator=(ArmorPack&&) = delete;

    [[nodiscard]] BonusKind getKind() const override;

private:
    void onApplyPlayer(Player& rPlayer) override;
    void onApplyBot(Bot& rBot) override;
};
