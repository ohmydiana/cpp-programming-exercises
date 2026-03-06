// Vaším úkolem je implementovat funkci find_shortest_path, která nalezne rychlejší procházku z libovolné ze vstupních místnosti entrances k pokladu treasure a zpět do libovolné vstupní místnosti, na které hrdina přežije.
// Pokud taková procházka neexistuje, vraťte prázdný vektor. Parametry funkce find_shortest_path jsou:
//
// const std::vector<Room>& rooms – seznam všech místností. Datovým typ Room je struktura obsahující:
//
// std::vector<RoomId> neighbors – seznam indexů místností (do pole rooms), do kterých z této vede chodba.
//
// std::optional<Monster> monster – obsahuje příšeru, pokud se v místnosti nějaká nachází. Datový typ Monster obsahuje atributy int hp, off, def, popisující počet životů, útok a obranu dané příšery a dále int stacking_off, stacking_def, které popisují, jak se změní útok, resp. obrana příšery po každém jejím tahu.
// Pokud hrdina vstoupí do místnosti s příšerou, tak započne souboj. Souboj končí jedině smrtí jednoho z účastníků (tedy pokud ani jeden ze soupeřů nedokáže druhého zranit, jsou oba lapeni v boji navždy). První ránu se pokusí zasadit příšera, pak hrdina, pak znovu příšera atd.
// Po každém útoku utrží cíl zranění rovné max(0, a_off - t_def), kde a_off je útok útočníka, t_def je obrana cíle. Cíl zemře, pokud má po útoku 0 nebo méně životů.
//
// Při rozumně efektivní implementaci zbylých částí úlohy je dodaná funkce simulate_combat dostatečně efektivní pro plný bodový zisk.
//
// Jelikož jsou chodby velmi dlouhé, tak lze čas strávený soubojem a výměnou předmětů zanedbat, hrdina se cestou do další místnosti zcela vyléčí a také, pokud hrdina znovu navštíví nějakou místnost, bude v ní znovu příšera i veškeré předměty.
//
// std::vector<Item> items – předměty, které se v místnosti nachází (popis struktury Item vizte níže). Pokud je v místnosti příšera, může je hrdina sebrat až po vítězství nad ní.
//
// const std::vector<RoomId>& entrances – indexy místností v poli rooms, kudy lze vejít do bludiště či z něj vyjít.
//
// RoomId treasure – index místnosti s pokladem, který chce hrdina sebrat.
//
// Návratovým typem je std::vector<Action> – seznam akcí začínající a končící v místnosti, která je vchodem (ale tyto místnosti mohou být různé). Typ Action je std::variant<Move, Pickup, Drop>. Hodnota typu Move znamená přesun do dané místnosti (musí sousedit s aktuální).
// Hodnota typu Pickup znamená sebrání předmětu v aktuální místnosti (musí se tam nacházet, pokud hrdina drží jiný předmět stejného typu, tak ho automaticky zahodí). Hodnota typu Drop znamená zahození drženého předmětu daného typu.
//
// Datový typ Item obsahuje atributy:
//
// Type type – druh předmětu. Existují 3 druhy: zbraně, brnění a gumové kačenky. Hrdina může v jednu chvíli mít nejvýše jeden předmět od každého druhu. Předmětu se může kdykoliv vyjma boje zbavit, ale tím je daný předmět ztracen, nelze ho následně sebrat (pokud nebude v místnosti nalezena nová kopie).
// std::string name – název předmětu (ne nutně unikátní).
// int hp, off, def – hodnoty, o které se změní hrdinovy životy, útok a obrana, pokud má daný předmět u sebe. Bonusy mohou být i záporné. Výsledná obrana a útok mohou být záporné. Pokud by maximální životy měly vyjít nekladné, bude mít hrdina 1 život.
// int stacking_off, stacking_def – hodnoty, o které se změní hrdinův útok, resp. obrana po každém jeho tahu. Tyto úpravy trvají do konce daného souboje.
// bool first_attack – pokud je true, souboj začíná hrdina a ne příšera.
// bool stealth – pokud je true, hrdina se může (ale nemusí) místností proplížit, čímž se vyhne souboji s příšerou. Pokud se hrdina plíží, nemůže sbírat věci a to ani hledaný poklad.
// Různých předmětů je v bludišti málo, ale stejný předmět se může vyskytovat v mnoha místnostech. Hrdina bez vybavení má 10000 životů, útok 3, obranu 2.

#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <functional>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <compare>
#include <ranges>
#include <optional>
#include <variant>

struct Item {
  enum Type : uint8_t {
    Weapon = 0,
    Armor = 1,
    RubberDuck = 2,
    TYPE_COUNT = 3,
  };

  std::string name;
  Type type;
  int hp = 0, off = 0, def = 0;
  int stacking_off = 0, stacking_def = 0;
  bool first_attack = false; // Hero attacks first.
  bool stealth = false; // Hero can sneak past monsters (but cannot loot items while sneaking).

  friend auto operator <=> (const Item&, const Item&) = default;
};

struct Monster {
  int hp = 0, off = 0, def = 0;
  int stacking_off = 0, stacking_def = 0;
};

using RoomId = size_t;
using ItemId = size_t;

struct Room {
  std::vector<RoomId> neighbors;
  std::optional<Monster> monster;
  std::vector<Item> items;
};

struct Move { RoomId room; };
struct Pickup { ItemId item; };
struct Drop { Item::Type type; };
using Action = std::variant<Move, Pickup, Drop>;

namespace student_namespace {
#endif

std::optional<int> turns_to_kill(int hp, int dmg, int stacking_dmg) {
  assert(hp > 0);

  if (stacking_dmg == 0) {
    if (dmg <= 0) return {};
    return (hp + dmg - 1) / dmg;
  }

  int i = 0;
  for (; hp > 0; i++) {
    if (dmg <= 0 && stacking_dmg < 0) return {};
    hp -= std::max(dmg, 0);
    dmg += stacking_dmg;
  }

  return i;
}

enum CombatResult {
  A_WINS, B_WINS, TIE
};

// Monster `a` attacks first
CombatResult simulate_combat(Monster a, Monster b) {
  a.def += a.stacking_def;

  auto a_turns = turns_to_kill(b.hp, a.off - b.def, a.stacking_off - b.stacking_def);
  auto b_turns = turns_to_kill(a.hp, b.off - a.def, b.stacking_off - a.stacking_def);

  if (!a_turns && !b_turns) return TIE;
  if (!a_turns) return B_WINS;
  if (!b_turns) return A_WINS;
  return *a_turns <= *b_turns ? A_WINS : B_WINS;
}

struct State
{
    RoomId room;
    std::optional<Item> weapon;
    std::optional<Item> armor;
    std::optional<Item> duck;
    bool has_treasure;
    bool in_stealth;

    bool operator<(const State & other) const
    {
        if (room != other.room) return room < other.room;
        if (has_treasure != other.has_treasure) return has_treasure < other.has_treasure;
        if (in_stealth != other.in_stealth) return in_stealth < other.in_stealth;
        if (weapon != other.weapon) return weapon < other.weapon;
        if (armor != other.armor) return armor < other.armor;
        return duck < other.duck;
    }

    bool operator==(const State & other) const
    {
        return room == other.room &&
               has_treasure == other.has_treasure &&
               in_stealth == other.in_stealth &&
               weapon == other.weapon &&
               armor == other.armor &&
               duck == other.duck;
    }
};

static Monster make_hero (const State & s)
{
    Monster hero {.hp = 10000, .off = 3, .def = 2};

    for (const auto & item : {s.weapon, s.armor, s.duck})
    {
        if (item)
        {
            hero.hp += item->hp;
            hero.off += item->off;
            hero.def += item->def;
            hero.stacking_off += item->stacking_off;
            hero.stacking_def += item->stacking_def;
        }
    }

    if (hero.hp <= 0) hero.hp = 1;
    return hero;
}

static bool can_defeat_monster (const std::vector<Room> & rooms, const State & s)
{
    if (!rooms[s.room].monster) return true;

    Monster hero = make_hero(s);
    Monster monster = *rooms[s.room].monster;

    bool hero_first = false;
    for (const auto & item : {s.weapon, s.armor, s.duck})
    {
        if (item && item->first_attack)
        {
            hero_first = true;
            break;
        }
    }

    CombatResult result;
    if (hero_first)
    {
        result = simulate_combat(hero, monster);
    }
    else
    {
        result = simulate_combat(monster, hero);
        result = (result == A_WINS) ? B_WINS : ((result == B_WINS) ? A_WINS : TIE);
    }

    return result == A_WINS;
}

static bool has_stealth (const State & s)
{
    for (const auto & item : {s.weapon, s.armor, s.duck})
    {
        if (item && item->stealth) return true;
    }
    return false;
}

static void pickups (const std::vector<Room> & rooms, const State & current, std::map<State, std::pair<State, Action>> & parent, std::deque<State> & dq)
{
    if (current.in_stealth) return;

    for (size_t i = 0; i < rooms[current.room].items.size(); i++)
    {
        const Item & item = rooms[current.room].items[i];
        State next = current;

        if (item.type == Item::Weapon)
        {
        	if (current.weapon) continue;
        	next.weapon = item;
        }
        else if (item.type == Item::Armor)
        {
        	if (current.armor) continue;
        	next.armor = item;
        }
        else if (item.type == Item::RubberDuck)
        {
        	if (current.duck) continue;
        	next.duck = item;
        }

        if (parent.count(next)) continue;

        parent[next] = {current, Pickup{i}};
        dq.push_front(next);
    }
}

static void drops (const State & current, const std::vector<Room> & rooms, std::map<State, std::pair<State, Action>> & parent, std::deque<State> & dq)
{
	if (current.weapon)
	{
		State next = current;
		next.weapon.reset();
		if (!parent.count(next))
		{
			parent[next] = {current, Drop{Item::Weapon}};
			dq.push_front(next);
		}
	}

	if (current.armor)
	{
		State next = current;
		next.armor.reset();
		if (!parent.count(next))
		{
			parent[next] = {current, Drop{Item::Armor}};
			dq.push_front(next);
		}
	}

	if (current.duck)
	{
		State next = current;
		next.duck.reset();
		if (!parent.count(next))
		{
			parent[next] = {current, Drop{Item::RubberDuck}};
			dq.push_front(next);
		}
	}
}

static void moves (const std::vector<Room> & rooms, const State & current, std::map<State, std::pair<State, Action>> & parent, std::deque<State> & dq, RoomId treasure)
{
    for (RoomId neighbour : rooms[current.room].neighbors)
    {
        State next = current;
        next.room = neighbour;
        next.in_stealth = false;

        bool can_enter = false;

        if (!rooms[neighbour].monster)
        {
            can_enter = true;
        }
        else if (can_defeat_monster(rooms, next))
        {
            can_enter = true;
        }
        else if (has_stealth(current))
        {
            can_enter = true;
            next.in_stealth = true;
        }

        if (!can_enter) continue;

    	if (neighbour == treasure && !current.has_treasure && !next.in_stealth)
    	{
    		next.has_treasure = true;
    	}

    	if (parent.count(next)) continue;

    	parent[next] = {current, Move{neighbour}};
    	dq.push_back(next);
    }
}

static std::pair<std::vector<Action>, RoomId> reconstruct_path(std::map<State, std::pair<State, Action>> & parent, const State & final_state)
{
    std::vector<Action> path;
    State s = final_state;
	RoomId start_room = s.room;

    while (true)
    {
        auto it = parent.find(s);
        if (it == parent.end()) break;

        auto [prev_state, action] = it->second;

        if (prev_state == s)
        {
        	start_room = s.room;
            break;
        }

        path.push_back(action);
        s = prev_state;
    }

    std::reverse(path.begin(), path.end());
    return {path, start_room};
}

std::vector<Action> find_shortest_path(const std::vector<Room>& rooms, const std::vector<RoomId>& entrances, RoomId treasure)
{
    std::deque<State> dq;
    std::map<State, std::pair<State, Action>> parent;

    if (rooms.empty()) return {};
    if (entrances.empty()) return {};

    for (RoomId entrance : entrances)
    {
        State start = {entrance, {}, {}, {}, (entrance == treasure), false};

        if (!can_defeat_monster(rooms, start)) continue;

        parent[start] = {start, {}};
        dq.push_back(start);
    }

    State final_state;
    bool found_solution = false;

    while (!dq.empty() && !found_solution)
    {
        State current = dq.front();
        dq.pop_front();

    	if (current.has_treasure)
    	{
    		for (RoomId entrance : entrances)
    		{
    			if (current.room == entrance && current.has_treasure)
    			{
    				final_state = current;
    				found_solution = true;
    				break;
    			}
    		}
    	}

    	if (found_solution) break;

    	pickups(rooms, current, parent, dq);
    	drops(current, rooms, parent, dq);
        moves(rooms, current, parent, dq, treasure);
    }

    if (!found_solution) return {};

    auto [path, start_room] = reconstruct_path(parent, final_state);

    path.insert(path.begin(), Move{start_room});

    return path;
}

#ifndef __PROGTEST__
}

bool contains(const auto& vec, const auto& x) {
  return std::ranges::find(vec, x) != vec.end();
};

#define CHECK(cond, ...) do { \
    if (!(cond)) { fprintf(stderr, __VA_ARGS__); assert(0); } \
  } while (0)
void check_solution(
  const std::vector<Room>& rooms,
  const std::vector<RoomId>& entrances,
  RoomId treasure,
  size_t expected_rooms,
  bool print = false
) {
  // TODO check if hero survives combat
  // TODO check if treasure was collected

  using student_namespace::find_shortest_path;
  const std::vector<Action> solution = find_shortest_path(rooms, entrances, treasure);

  if (expected_rooms == 0) {
    CHECK(solution.size() == 0, "No solution should exist but got some.\n");
    return;
  }

  CHECK(solution.size() != 0, "Expected solution but got none.\n");

  try {
    CHECK(contains(entrances, std::get<Move>(solution.front()).room),
      "Path must start at entrance.\n");
    CHECK(contains(entrances, std::get<Move>(solution.back()).room),
      "Path must end at entrance.\n");
  } catch (const std::bad_variant_access&) {
    CHECK(false, "Path must start and end with Move.\n");
  }

  std::vector<Item> equip;
  RoomId cur = std::get<Move>(solution.front()).room;
  CHECK(cur < rooms.size(), "Room index out of range.\n");
  size_t room_count = 1;
  if (print) printf("Move(%zu)", cur);

  auto drop_items = [&](Item::Type type) {
    std::erase_if(equip, [&](const Item& i) { return i.type == type; });
  };

  for (size_t i = 1; i < solution.size(); i++) {
    if (auto m = std::get_if<Move>(&solution[i])) {
      CHECK(m->room < rooms.size(), "Next room index out of range.\n");
      CHECK(contains(rooms[cur].neighbors, m->room),
        "Next room is not a neighbor of the current one.\n");
      cur = m->room;
      room_count++;

      if (print) printf(", Move(%zu)", cur);
    } else if (auto p = std::get_if<Pickup>(&solution[i])) {
      CHECK(p->item < rooms[cur].items.size(), "Picked up item out of range.\n");
      const Item& item = rooms[cur].items[p->item];
      drop_items(item.type);
      equip.push_back(item);

      if (print) printf(", Pickup(%zu, %s)", p->item, item.name.c_str());
    } else {
      auto t = std::get<Drop>(solution[i]).type;
      drop_items(t);

      if (print) printf(", Drop(%s)",
        t == Item::Armor ? "Armor" :
        t == Item::Weapon ? "Weapon" :
        t == Item::RubberDuck ? "Duck" :
        "ERROR");
    }
  }

  if (print) printf("\n");

  CHECK(room_count == expected_rooms,
    "Expected %zu rooms but got %zu.\n", expected_rooms, room_count);
}
#undef CHECK


void combat_examples() {
  const Item defensive_duck = {
    .name = "Defensive Duck", .type = Item::RubberDuck,
    .off = -2, .def = 8,
  };

  const Item invincible_duck = {
    .name = "Invincible Duck", .type = Item::RubberDuck,
    .hp = -20'000, .def = 1'000,
  };

  const Item fast_duck = {
    .name = "Fast Duck", .type = Item::RubberDuck,
    .first_attack = true,
  };

  const Item offensive_duck = {
    .name = "Offensive Duck", .type = Item::RubberDuck,
    .stacking_off = 100,
  };

  std::vector<Room> rooms(2);
  rooms[0].neighbors.push_back(1);
  rooms[1].neighbors.push_back(0);

  check_solution(rooms, { 0 }, 1, 3);

  rooms[1].monster = Monster{ .hp = 9'999, .off = 3, .def = 2 };
  check_solution(rooms, { 0 }, 1, 3);

  rooms[1].monster->hp += 1;
  check_solution(rooms, { 0 }, 1, 0);

  rooms[1].monster = Monster{ .hp = 100'000, .off = 10 };
  check_solution(rooms, { 0 }, 1, 0);

  rooms[0].items = { defensive_duck };
  check_solution(rooms, { 0 }, 1, 3);

  rooms[0].items = { invincible_duck };
  check_solution(rooms, { 0 }, 1, 3);

  rooms[0].items = {};
  rooms[1].monster = Monster{ .hp=1, .off=3, .def=0, .stacking_def=100 };
  check_solution(rooms, { 0 }, 1, 0);

  rooms[0].items.push_back(offensive_duck);
  check_solution(rooms, { 0 }, 1, 0);

  rooms[0].items.push_back(fast_duck);
  check_solution(rooms, { 0 }, 1, 3);
}

void stealth_examples() {
  const Item stealth_duck = {
    .name = "Stealth Duck", .type = Item::RubberDuck,
    .stealth = true,
  };

  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 10,
  };

  const Monster m = { .hp = 10'000, .off=10, .def=2 };

  std::vector<Room> rooms(4);

  for (size_t i = 1; i < rooms.size(); i++) {
    rooms[i].neighbors.push_back(i - 1);
    rooms[i - 1].neighbors.push_back(i);
  }

  rooms[0].items = { stealth_duck };
  rooms[2].monster = m;

  check_solution(rooms, { 0 }, 2, 0); // Cannot stealth steal treasure

  rooms[3].items = { sword };
  // Stealth to 3, grab sword & kill monster
  check_solution(rooms, { 0 }, 2, 7);

  rooms[3].items = {};
  rooms[1].items = { sword };
  check_solution(rooms, { 0 }, 2, 5);

  rooms[1].monster = m;
  check_solution(rooms, { 0 }, 2, 0); // Cannot pickup while stealthing
}

void example_tests() {
  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 10, .def = -1,
  };

  const Item berserker_sword = {
    .name = "Berserker's Sword", .type = Item::Weapon,
    .hp = -1'000, .off = 10'000, .def = 0,
    .stacking_off = 1'000, .stacking_def = -500,
    .first_attack = true
  };

  const Item heavy_armor = {
    .name = "Heavy Armor", .type = Item::Armor,
    .hp = 5'000, .off = -10, .def = 300,
  };

  const Item debugging_duck = {
    .name = "Debugging Duck", .type = Item::RubberDuck,
    .stacking_off = 1,
    .stealth = true
  };

  std::vector<Room> rooms(14);
  enum : RoomId {
    no_monster = 10,
    weak,
    strong,
    durable
  };

  rooms[no_monster] = { {}, {}, { heavy_armor } };
  rooms[weak] = { {}, Monster{ .hp = 1000, .off = 10 }, { debugging_duck, sword } };
  rooms[strong] = { {}, Monster{ .hp = 10, .off = 10'000, .def = 1'000'000 },
    { berserker_sword } };
  rooms[durable] =  { {}, Monster{ .hp = 100'000, .off = 10, .stacking_def = 1 },
    { berserker_sword } };

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  link(0, no_monster);
  link(0, weak);
  link(weak, 7);
  link(0, strong);
  link(strong, 8);
  link(0, 1);
  link(1, 2);
  link(2, durable);
  link(durable, 6);

  check_solution(rooms, { 0 }, 0, 1); // Treasure at entrance
  check_solution(rooms, { 9 }, 0, 0); // No path to treasure
  check_solution(rooms, { 8 }, 0, 0); // Blocked by monster
  check_solution(rooms, { durable }, durable, 0); // Killed on spot
  check_solution(rooms, { 7 }, 0, 5); // Kills weak monster
  check_solution(rooms, { 6, 7 }, 2, 7); // Sneaks around durable
  check_solution(rooms, { 6, 7 }, durable, 9); // Kills durable
}

void example_tests2() {
  const Item duck_of_power = {
    .name = "Duck of Power", .type = Item::RubberDuck,
    .hp = 10'000'000, .off = 10'000'000, .def = 10'000'000,
  };

  const Item dull_sword = {
    .name = "Dull Sword", .type = Item::Weapon,
    .off = -10, .def = -5,
  };

  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 5, .def = -1,
  };

  const Item leather_pants = {
    .name = "Leather pants", .type = Item::Armor,
    .off = -3, .def = 1,
    .first_attack = true
  };

  const Item defensive_duck = {
    .name = "Defensive Duck", .type = Item::RubberDuck,
    .off = -2, .def = 8,
  };

  const Item stealth_duck = {
    .name = "Stealth Duck", .type = Item::RubberDuck,
    .off = -100, .def = -100,
    .stealth = true,
  };

  const Item slow_sword = {
    .name = "Slow Sword", .type = Item::Weapon,
    .off = -10'000,
    .stacking_off = 1,
  };

  constexpr int CYCLE_LEN = 100;
  enum : RoomId {
    impossible = CYCLE_LEN,
    r1, r2, r3, r4, r4a, r4b, ROOM_COUNT
  };
  std::vector<Room> rooms(ROOM_COUNT);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  for (int i = 1; i < CYCLE_LEN; i++) link(i - 1, i);
  rooms[CYCLE_LEN-1].neighbors.push_back(0);

  rooms[impossible] = { {}, {{ .hp = 1'000'000, .off = 1'000'000 }}, { duck_of_power } };
  link(impossible, 0);

  rooms[r1] = { {}, {{ .hp = 9'999, .off = 3, .def = 2 }}, { defensive_duck, dull_sword } };
  link(r1, 1);

  rooms[r2] = { {}, {{ .hp = 100'000, .off = 10 }}, { sword, leather_pants } };
  link(r2, CYCLE_LEN - 3);

  rooms[r3] = { {}, {{ .hp = 100'000, .off = 10, .def = 1 }}, { stealth_duck, slow_sword } };
  link(r3, 2);

  rooms[r4] = { { r4a }, {{ .hp = 10'000, .off = 10'000 }}, {} };
  rooms[r4a] = { { r4b } };
  rooms[r4b] = { {}, {{ .hp = 10'000, .off = 1 }}, {} };
  link(r4, CYCLE_LEN - 4);
  link(r4b, CYCLE_LEN - 4);

  // r1 (loots duck) -> r2 (loots pants & sword) -> r3
  check_solution(rooms, { 0 }, r3, CYCLE_LEN + 11);
  // r1 (loots duck) -> r2 (loots pants & sword) -> r3 (loots stealth duck) -> r4a
  check_solution(rooms, { 0 }, r4a, 2*CYCLE_LEN + 11);
}

void example_tests3() {
  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 10,
  };

  const Item stacking_duck = {
    .name = "Stacking Duck", .type = Item::RubberDuck,
    .hp = -9'999, .stacking_off = 100,
  };

  const Item heavy_armor = {
    .name = "Heavy Armor", .type = Item::Armor,
    .off = -1'000, .def = 1'000,
  };

  enum : RoomId {
    start, treasure, short_path, long_path = short_path + 3,
    COUNT = long_path + 4
  };

  std::vector<Room> rooms(COUNT);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  rooms[treasure].neighbors.push_back(start); // one-way back to start

  link(start, long_path + 0);
  link(long_path + 0, long_path + 1);
  link(long_path + 1, long_path + 2);
  link(long_path + 2, long_path + 3);
  link(long_path + 3, treasure);

  link(start, short_path + 0);
  link(short_path + 0, short_path + 1);
  link(short_path + 1, short_path + 2);
  link(short_path + 2, treasure);

  rooms[short_path + 0].items = { sword };
  rooms[short_path + 1].monster = Monster{ .hp=10'000, .off=5, .def=3 };
  rooms[short_path + 1].items = { stacking_duck, heavy_armor };
  rooms[short_path + 2].monster = Monster{ .hp=100'000, .off=5, .def=3 };

  check_solution(rooms, { start }, treasure, 6);
}

void example_tests4() {
  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 3, .def = -1
  };

  enum : RoomId {
    start, treasure, short_path, long_path = short_path + 3,
    COUNT = long_path + 4
  };

  std::vector<Room> rooms(COUNT);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  rooms[treasure].neighbors.push_back(start); // one-way back to start

  link(start, long_path + 0);
  link(long_path + 0, long_path + 1);
  link(long_path + 1, long_path + 2);
  link(long_path + 2, long_path + 3);
  link(long_path + 3, treasure);

  link(start, short_path + 0);
  link(short_path + 0, short_path + 1);
  link(short_path + 1, short_path + 2);
  link(short_path + 2, treasure);

  Monster needs_sword = Monster{ .hp=10'000, .off=6, .def=3 };
  Monster no_sword = Monster{ .hp=100'000, .off=3 };
  rooms[short_path + 0].monster = needs_sword;
  rooms[short_path + 1].monster = no_sword;
  rooms[short_path + 0].monster = needs_sword;

  check_solution(rooms, { start }, treasure, 7);
}

void example_tests5() {
  const Item sword = {
    .name = "Sword", .type = Item::Weapon,
    .off = 5, .def = -1,
  };

  constexpr int LEN = 300;
  std::vector<Room> rooms(LEN);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  for (int i = 1; i < LEN; i++) {
    rooms[i].items = { sword, sword, sword };
    link(i - 1, i);
  }

  rooms[LEN - 1].monster = Monster{ .hp = 1'000'000, .off = 1'000'000 };

  check_solution(rooms, { 0 }, LEN - 1, 0);
}

void example_tests6() {
  const Item defensive_duck = {
    .name = "Defensive Duck", .type = Item::RubberDuck,
    .off = -100, .def = 100,
  };

  constexpr int LEN = 31;
  std::vector<Room> rooms(LEN + LEN + 10);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  rooms[0].items = { defensive_duck };

  assert(LEN % 2 == 1);
  for (int i = 1; i + 1 < LEN; i += 2) {
    link(i - 1, i);
    link(i, i + 1);

    rooms[i+1].items = { defensive_duck };

    rooms[i].monster = Monster{ .hp = 10'000'000, .off = 50, .def = -120 };
    rooms[i+1].monster = Monster{ .hp = 10'000, .off = 1, .def = 1 };
  }

  for (int i = 1; i < LEN + 10; i++)
    link(LEN + i - 1, LEN + i);

  link(0, LEN);
  link(LEN - 1, 2*LEN + 10 - 1);

  check_solution(rooms, { 0 }, LEN - 1, 2*LEN - 1);
}

void example_tests7() {
  const Item stealth_underpants = {
    .name = "Stealth Underpants", .type = Item::Armor,
    .off = 0, .def = 0,
    .stealth = true,
  };

  const Item offensive_underpants = {
    .name = "Offensive Underpants", .type = Item::Armor,
    .hp = -10'000, .off = 100,
  };

  const int LEN = 4;
  std::vector<Room> rooms(4);

  auto link = [&](RoomId a, RoomId b) {
    rooms[a].neighbors.push_back(b);
    rooms[b].neighbors.push_back(a);
  };

  for (int i = 1; i < LEN; i++) link(i - 1, i);

  rooms[2].items = { offensive_underpants };

  rooms[1].monster = Monster{ .hp = 1, .off = 1, .def = 50 };
  rooms[1].items = { stealth_underpants };

  rooms[0].monster = Monster{ .hp=100'000, .off=1000, .def=1000 };
  check_solution(rooms, { 0, LEN - 1 }, 1, LEN);
}


int main() {
  combat_examples();
  stealth_examples();
  example_tests();
  example_tests2();
  example_tests3();
  example_tests4();
  example_tests5();
  example_tests6();
  example_tests7();
}

#endif