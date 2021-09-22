#include "game/sea-battle.h"
#include "game/field.h"

sea_battle_t::sea_battle_t(std::shared_ptr<player_interface_t> player1, field_t field1,
                           std::shared_ptr<player_interface_t> player2, field_t field2)
    : player1{std::move(player1)}, player2{std::move(player2)}, field1{std::move(field1)}, field2{std::move(field2)} {}

void hide_ships(field_t &field) {
  for (int i = 0; i < field_t::FIELD_SIZE; ++i) {
    std::replace(field[i].begin(), field[i].end(), field_t::SHIP_CELL, field_t::EMPTY_CELL);
  }
}

void sea_battle_t::play() {
  while (true) {
    auto player = (current_turn == FIRST_PLAYER) ? player1 : player2;
    auto enemy = (current_turn == FIRST_PLAYER) ? player2 : player1;
    auto player_field = (current_turn == FIRST_PLAYER) ? field1 : field2;
    auto &enemy_field = (current_turn == FIRST_PLAYER) ? field2 : field1;
    field_t enemy_field_copy(enemy_field);
    hide_ships(enemy_field_copy);

    auto pos = player->make_move(player_field, enemy_field_copy);
    int x = pos.first;
    int y = pos.second;

    if (not field_t::is_cell_valid(x, y)) {
      player->on_incorrect_move(x, y);
      continue;
    }
    if (enemy_field[x][y] == field_t::MISS_CELL or enemy_field[x][y] == field_t::HIT_CELL) {
      player->on_duplicate_move(x, y);
      continue;
    }
    if (enemy_field[x][y] == field_t::EMPTY_CELL) {
      enemy_field[x][y] = field_t::MISS_CELL;
      player->on_miss(x, y);
      current_turn = change_turn(current_turn);
      continue;
    }
    if (enemy_field[x][y] == field_t::SHIP_CELL) {
      enemy_field[x][y] = field_t::HIT_CELL;
      for (const auto &dir : field_t::DIRECTIONS) {
        if (field_t::is_cell_valid(x + dir.first, y + dir.second) and
            (enemy_field[x + dir.first][y + dir.second] == field_t::SHIP_CELL or
             (enemy_field[x + dir.first][y + dir.second] == field_t::HIT_CELL))) {
          for (int i = 1; i <= field_t::DIRECTIONS.size(); ++i) {
            int x_offset = x + dir.first * i;
            int y_offset = y + dir.second * i;
            if (field_t::is_cell_valid(x_offset, y_offset)) {
              if (enemy_field[x_offset][y_offset] == field_t::HIT_CELL) {
                continue;
              } else if (enemy_field[x_offset][y_offset] == field_t::SHIP_CELL) {
                player->on_hit(x, y);
                goto outer; // continue outer while loop iteration
              } else if (enemy_field[x_offset][y_offset] == field_t::EMPTY_CELL or
                         enemy_field[x_offset][y_offset] == field_t::MISS_CELL) {
                break;
              }
            }
          }
        }
      }
      player->on_kill(x, y);
    }

    for (int i = 0; i < field_t::FIELD_SIZE; ++i) {
      if (std::any_of(enemy_field[i].begin(), enemy_field[i].end(),
                      [](char cell) { return cell == field_t::SHIP_CELL; })) {
        goto outer; // continue outer while loop iteration
      }
    }
    player->on_win();
    enemy->on_lose();
    break;
  outer:
    continue;
  }
}

sea_battle_t::turn_t sea_battle_t::change_turn(turn_t current_turn) {
  return current_turn == FIRST_PLAYER ? SECOND_PLAYER : FIRST_PLAYER;
}

std::string sea_battle_t::get_player_name(turn_t turn) {
  return turn == FIRST_PLAYER ? "First" : "Second";
}