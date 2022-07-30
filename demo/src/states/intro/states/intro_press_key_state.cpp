/*
# ______       ____   ___
#   |     \/   ____| |___|
#   |     |   |   \  |   |
#-----------------------------------------------------------------------
# Copyright 2022, tyra - https://github.com/h4570/tyra
# Licenced under Apache License 2.0
# Sandro Sobczyński <sandro.sobczynski@gmail.com>
*/

#include "states/intro/states/intro_press_key_state.hpp"
#include "loaders/texture/png_loader.hpp"
#include "file/file_utils.hpp"
#include "debug/debug.hpp"
#include <string>
#include "thread/threading.hpp"
#include "math/math.hpp"

using Tyra::FileUtils;
using Tyra::Math;
using Tyra::PngLoader;
using Tyra::SpriteMode;
using Tyra::Threading;

namespace Demo {

IntroPressKeyState::IntroPressKeyState(Engine* t_engine)
    : State(t_engine), mapPosition(0.0F, 0.0F) {
  state = STATE_PRESS_KEY;
  mapDirection = 0;
  _wantFinish = false;
}

IntroPressKeyState::~IntroPressKeyState() {
  if (!initialized) return;

  for (u8 i = 0; i < mapRows; i++)
    for (u8 j = 0; j < mapCols; j++) {
      engine->renderer.core.texture.repository.free(mapTextures[i][j]->getId());
      delete mapSprites[i][j];
    }
}

void IntroPressKeyState::onStart() {
  for (u8 i = 0; i < mapRows; i++) {
    for (u8 j = 0; j < mapCols; j++) {
      mapSprites[i][j] = new Sprite;
      mapSprites[i][j]->setMode(SpriteMode::MODE_STRETCH);
      mapSprites[i][j]->size.set(textureWidthHeight, textureWidthHeight);
      mapSprites[i][j]->position.set(i * textureWidthHeight,
                                     j * textureWidthHeight);

      std::string filename =
          "map-" + std::to_string(i) + "-" + std::to_string(j);
      std::string path = "intro/map/" + filename + ".png";

      mapTextures[i][j] = engine->renderer.core.texture.repository.add(
          FileUtils::fromCwd(path));
      mapTextures[i][j]->addLink(mapSprites[i][j]->getId());
    }
  }

  initialized = true;
}

void IntroPressKeyState::update() {
  engine->renderer.beginFrame();

  if (mapDirection == 0 &&
      mapPosition.x >= textureWidthHeight * (mapRows - 1) &&
      mapPosition.y >= textureWidthHeight * (mapCols - 1)) {
    mapDirection = 1;
  } else if (mapDirection == 1 && mapPosition.x <= 0.0F &&
             mapPosition.y <= 0.0F) {
    mapDirection = 0;
  }

  const float offset = 0.1F;
  if (mapDirection == 0) {
    mapPosition += offset;
  } else if (mapDirection == 1) {
    mapPosition -= offset;
  }

  updateMap();
  Threading::switchThread();

  for (u8 i = 0; i < mapRows; i++)
    for (u8 j = 0; j < mapCols; j++)
      engine->renderer.renderer2D.render(mapSprites[i][j]);

  engine->renderer.endFrame();
}

IntroStateType IntroPressKeyState::onFinish() { return STATE_INTRO_END; }

void IntroPressKeyState::updateMap() {
  for (u8 i = 0; i < mapRows; i++)
    for (u8 j = 0; j < mapCols; j++) {
      mapSprites[i][j]->position.x = i * textureWidthHeight - mapPosition.x;
      mapSprites[i][j]->position.y = j * textureWidthHeight - mapPosition.y;
    }
}
}  // namespace Demo
