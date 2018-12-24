#include <cassert>
#include "map.hpp"

/*
 * Function to test new Map implementation
 */
int main() {

    // Testing default constructor --- double as Key
    nm::Map<double, double> map0_1;
    assert(map0_1.empty());
    map0_1.insert({2.2, 30.0});
    map0_1.insert({1.1, 29.0});
    assert(!map0_1.empty());

    // Testing default constructor --- string as Key (with duplicate keys)
    nm::Map<std::string, double> map0_2;
    assert(map0_2.empty());
    map0_2.insert({"Mishra", 29.9});
    map0_2.insert({"Nitesh", 30.9});
    assert(!(map0_2.insert({"Nitesh", 40.9}).second));
    assert(map0_2.size() == 2);
    for (auto iter = map0_2.begin(); iter != map0_2.end();) {
        map0_2.erase(iter); // Test erase function
        iter = map0_2.begin();
    }
    assert(map0_2.size() == 0);
    assert(map0_2.begin() == map0_2.end());


    // Testing copy constructor --- string as Key
    nm::Map<std::string, int> map1_1;
    assert(map1_1.empty());
    map1_1.insert({"Mishra", 29});
    map1_1.insert({"Nitesh", 30});
    nm::Map<std::string, int> map1_2(map1_1);
    assert(!map1_2.empty());

    // Testing copy constructor --- string as Key
    nm::Map<int, std::string> map2_1;
    assert(map2_1.empty());
    map2_1.insert({30, "Nitesh"});
    map2_1.insert({29, "Mishra"});
    nm::Map<int, std::string> map2_2{map2_1};
    assert(!map2_2.empty());

    // Testing initializer list --- string as Key
    nm::Map<std::string, std::string> map3_1{{"Last Name",  "Mishra"},
                                             {"First Name", "Nitesh"}};
    nm::Map<std::string, std::string> map3_2{map3_1};
    assert(!map3_1.empty());
    assert(!map3_2.empty());

    // Testing assignment operator --- boolean as Key
    nm::Map<bool, std::string> map4_1{{true,  "TRUE"},
                                      {false, "FALSE"}};
    nm::Map<bool, std::string> map4_2{{true, "NITESH"}};
    map4_1 = map4_1; // Self assignment
    map4_2 = map4_1; // Different assignment
    assert(!map4_1.empty());
    assert(!map4_2.empty());

    // Testing range insert --- initializer list
    std::initializer_list<std::pair<const std::string, float>> init_list{{"Z", 30},
                                                                         {"P", 20},
                                                                         {"A", 10}};
    nm::Map<std::string, float> map5_1;
    assert(map5_1.empty());
    map5_1.insert(init_list.begin(), init_list.end());
    nm::Map<std::string, float> map5_2;
    map5_2 = map5_1;
    assert(map5_1.size() == 3);
    assert(map5_1.size() == map5_2.size());

    // Testing erase functionality --- passing Key as parameter
    nm::Map<float, std::string> map6_1{{6.0, "PST"},
                                       {5.0, "OOPS"},
                                       {4.0, "OS"},
                                       {3.0, "COA"},
                                       {2.0, "PL"},
                                       {1.0, "DAA"}};
    assert(map6_1.size() == 6);
    map6_1.erase(1.0);
    assert(map6_1.size() == 5);
    map6_1.erase(6.0);
    assert(map6_1.size() == 4);
    map6_1.erase(3.0);
    assert(map6_1.size() == 3);
    map6_1.erase(2.0);
    map6_1.erase(4.0);
    map6_1.erase(5.0);
    assert(map6_1.empty());

    // Testing erase functionality --- passing Iterator as parameter
    nm::Map<float, std::string> map7_1{{6.0, "PST"},
                                       {5.0, "OOPS"},
                                       {4.0, "OS"},
                                       {3.0, "COA"},
                                       {2.0, "PL"},
                                       {1.0, "DAA"}};
    assert(map7_1.size() == 6);
    nm::Map<float, std::string>::Iterator iter7_1 = map7_1.begin();
    map7_1.erase(iter7_1);
    assert(map7_1.begin()->first == 2.0);
    assert(map7_1.size() == 5);
    // Testing with Malicious iterator
    nm::SkipNode<float, std::string> *node7_1 = new nm::SkipNode<float, std::string>(1, {2.0, "DB"});
    nm::Map<float, std::string>::Iterator iter7_2(node7_1);
    map7_1.erase(iter7_2);
    assert(map7_1.begin()->first == 2.0);
    assert(map7_1.size() == 5);
    delete node7_1;

    // Testing clear functionality
    nm::Map<std::string, char> map8_1{{"PST",  'A'},
                                      {"OOPS", 'T'},
                                      {"OS",   'T'},
                                      {"COA",  'A'},
                                      {"PL",   'T'},
                                      {"DAA",  'A'}};
    assert(map8_1.size() == 6);
    map8_1.clear();
    assert(map8_1.empty());
    map8_1.insert({"PST", 'A'});
    assert(map8_1.size() == 1);

    std::cout << "\nTest completed successfully !!\n" << std::endl;

    return 0;
}
