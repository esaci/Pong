#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

class entity{
    public:
        int id; // Unique identifier
        int type; // 0=monster, 1=your hero, 2=opponent hero
        int x; // Position of this entity
        int y;
        int shield_life; // Ignore for this league; Count down until shield spell fades
        int is_controlled; // Ignore for this league; Equals 1 when this entity is under a control spell
        int health; // Remaining health of this monster
        int vx; // Trajectory of this monster
        int vy;
        int near_base; // 0=monster with no target yet, 1=monster targeting a base
        int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
        entity(std::vector<int> &tab){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];
        }
};

class base{
    public:
        int base_x; // The corner of the map representing your base
        int base_y;
        int max_x;
        int max_y;
        int health;
        int mana;
        std::map<int, entity*> order_defense;
    public:
        base(int x, int y): base_x(x), base_y(y){
            max_x = 300;
            if (x != 0)
                max_x = x - 300;
            max_y = 300;
            if (y != 0)
                max_y = y - 300;
            std::cerr << "La base se trouve dans (" << x << " - " << max_x << ", " << y << " - " << max_y << ")" << std::endl;
        }
        ~base(){for(std::map<int, entity*>::iterator it = order_defense.begin(); it != order_defense.end();it++)delete (it)->second;}
        void sethm(int h, int m){health = h; mana = m;}
        
        int to_a_b_distance(entity *arg){
            int res = 0, x = arg->x, y = arg->y;
            for (; x > 0 && y > 0 && x < 17631 && y < 9001;res++, x += (arg->vx / 10), y += (arg->vy / 10))
            {
				if (pow(x - base_x, 2) + pow(y - base_y, 2) < pow(5000, 2))
                    return (res);
            }
            return(-1);
        }

        int to_b_distance(entity *arg){
            int res = 0, x = arg->x, y = arg->y;
            for (; x > 0 && y > 0 && x < 17631 && y < 9001;res++, x += (arg->vx / 10), y += (arg->vy / 10))
            {
				if (pow(x - base_x, 2) + pow(y - base_y, 2) < pow(300, 2))
                    return (res);
            }
            return(-1);
        }
        void add_to_def(entity *arg){
            int res = to_b_distance(arg);
            if (res != -1)
            {
                order_defense[res] = arg;
                return ;
            }
            res = to_a_b_distance(arg);
            if (res == -1)
            {
                delete arg;
                return;
            }
            order_defense[res + 190] = arg;
        }

};

int main()
{
    int x, y;
    int heroes_per_player; // Always 3
    int entity_count; // Amount of heros and monsters you can see
    entity *tmp;
    std::map<int, std::vector<entity *> > threat_base;
    std::vector<int> tab;

    tab.reserve(20);
    std::cin >> x >> y; std::cin.ignore();
    base base_ally(x, y), base_ennemy(-x, -y);
    std::cin >> heroes_per_player; std::cin.ignore();
    while (1) {
        for (int i = 0; i < 2; i++) {
            std::cin >> x >> y; std::cin.ignore();
            base_ally.sethm(x, y);
            // std::cin >> x >> y; 
            // base_ennemy.sethm(x, y);
        }
        std::cin >> entity_count; std::cin.ignore();
        for (int i = 0; i < entity_count; i++) {
            x = 0;
            std::cin >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x]; std::cin.ignore();
            tmp = new entity(tab);

            if (!tmp->type)
                base_ally.add_to_def(tmp);
            // if (!tmp->type && base_ally.to_b_distance(tmp) >= 0)
                // std::cerr << "Adr mob " << tmp->x << ", " << tmp->y << "\n" << "(vx, vy) (" << tmp->vx << ", " << tmp->vy << ")\nArrive dans " << base_ally.to_b_distance(tmp) << std::endl;
            // delete tmp;  
        }
        std::map<int, entity *>::iterator it = base_ally.order_defense.begin();
        for (int i = 0; i < heroes_per_player; i++) {
            if (it != base_ally.order_defense.end())
            {
                std::cout << "MOVE " << it->second->x << " " << it->second->y << std::endl;
                base_ally.order_defense.erase(it);
                it = base_ally.order_defense.begin();
            }
            // In the first league: MOVE <x> <y> | WAIT; In later leagues: | SPELL <spellParams>;
            else
                std::cout << "WAIT" << std::endl;
        }
    }
}