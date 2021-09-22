#include "players/human-player.h"
#include <game/sea-battle.h>

std::pair<int, int> human_player_t::make_move(const field_t &my_field, const field_t &enemy_field) {
  output_stream << "Your field:" << std::endl;
  output_stream << my_field << std::endl;
  output_stream << "Enemy's field" << std::endl;
  output_stream << enemy_field << std::endl;
  output_stream << "Input you turn! [Example 1A or 10B or 6J]" << std::endl;

  std::string s;
  std::getline(input_stream, s);
  std::set cell_names{
    "1F", "3J", "8G", "4A", "8D", "8J", "7B", "7G", "3F", "6C", "6J", "7C", "1A", "6I", "10G", "6A", "1E", "7J", "6G",
        "4E", "2B", "2H", "6F", "4D", "1C", "4I", "2A", "5C", "3I", "5B", "7D", "9J", "7F", "2F", "1B", "9F", "10J",
        "6E", "5J", "10B", "9D", "3D", "7A", "10C", "7E", "3G", "7I", "1G", "3A", "5D", "5F", "4C", "9E", "2E", "6H",
        "5A", "5H", "8E", "4G", "3C", "4F", "1I", "8I", "8B", "6D", "4J", "5E", "10I", "3E", "8F", "1H", "2G", "4B",
        "9C", "9I", "6B", "2I", "8A", "4H", "9G", "9B", "10A", "1D", "8C", "9A", "10F", "2C", "2D", "3B", "1J", "3H",
        "10H", "10E", "8H", "5G", "7H", "2J", "5I", "10D", "9H"};
  while (std::find(cell_names.begin(), cell_names.end(), s) == cell_names.end()) {
    output_stream << "Incorrect move! [Correct example 1A or 10B or 6J]" << std::endl;
    std::getline(input_stream, s);
  }

  int x, y;
  if (s.length() == 2) {
    x = std::stoi(s) - 1;
  } else if (s.length() == 3) {
    x = field_t::FIELD_SIZE - 1;
  }
  y = s.back() - 65;

  return std::pair{x, y};
}

void human_player_t::on_incorrect_move(int x, int y) {
  assert(false);
}

std::string get_cell_name(int x, int y) {
  std::string cell_name;
  cell_name = std::to_string(x + 1);
  cell_name += static_cast<char>(y + 65);
  return cell_name;
}

void human_player_t::on_duplicate_move(int x, int y) {
  output_stream << get_cell_name(x, y) << " DUPLICATE! Repeat move!" << std::endl;
}

void human_player_t::on_miss(int x, int y) {
  output_stream << get_cell_name(x, y) << " MISS! Enemy's turn!" << std::endl;
}

void human_player_t::on_hit(int x, int y) {
  output_stream << get_cell_name(x, y) << " HIT! Your next move!" << std::endl;
}

void human_player_t::on_kill(int x, int y) {
  output_stream << get_cell_name(x, y) << " KILL! Your next move!" << std::endl;
}

void human_player_t::on_win() {
  output_stream << "You WIN!" << std::endl;
}

void human_player_t::on_lose() {
  output_stream << "You LOSE!" << std::endl;
}