base(int x, int y): base_x(x), base_y(y), mana(0), uusafe_dist_per_damage(100), usafe_dist_per_damage(300), safe_dist_per_damage(200), max_def(10000){
            max_x = !x ? 10000: x - 10000;
            max_y = !y ? 5000 : y - 5000;
            flag0_x = !x ? 2500 : x - 2500;
            flag0_y = !y ? 2500 : y - 2500;
            flag0_xn = flag0_x;
            flag0_yn = flag0_y;
            flag1_x = max_x;
            flag1_y = max_y;
            flag1_xn = flag1_x;
            flag1_yn = flag1_y;
            flag2_x = !x ? 17630 - 8000 : 8000;
            flag2_y = !y ? 9000 - 2000 : 2000;
            flag2_xn = flag2_x;
            flag2_yn = flag2_y;
            flag3_x = !x ? 17630 - 6000 : 6000;
            flag4_x = !x ? 17630 - 1000 : 1000;
            flag3_y = !y ? 9000 - 1000 : 1000 ;
            flag4_y = !y ? 9000 - 6000 : 6000 ;
            flag3_xn = flag3_x;
            flag3_yn = flag3_y;
            name.push_back("Mortimer");
            name.push_back("Randolph");
            name.push_back("J_J");
            ebase_x = 17630 - base_x,ebase_y = 9000 - base_y;
        }