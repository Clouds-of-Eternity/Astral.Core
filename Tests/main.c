#include <stdlib.h>
#include "HashMap.h"
#include "Strings.h"

#define XSTR(a) ISTR(a)
#define ISTR(a) #a

#define EXPECT_OR_RET(expr) if (!(expr)) { fprintf(stderr, "Error: (%s) at line %i", XSTR(expr), __LINE__); return false;}

bool TestHashMaps()
{
    HashMap hm = HashMap_Create(sizeof(string), sizeof(uint32_t), GetCAllocator(), &StringPtr_Hash, &StringPtr_Eqls);
    string key1 = StringFrom(GetCAllocator(), "Hello, World!");
    uint32_t val1 = 1;
    string key2 = StringFrom(GetCAllocator(), "Goodbye, World!");
    uint32_t val2 = 0;
    string key3 = StringFrom(GetCAllocator(), "He");
    uint32_t val3 = 4;
    string key4 = StringFrom(GetCAllocator(), "Hehe");
    uint32_t val4 = 4;
    string key5 = StringFrom(GetCAllocator(), "Hehehe");
    uint32_t val5 = 6;
    string key6 = StringFrom(GetCAllocator(), "Heheheha");
    uint32_t val6 = 7;
    HashMap_Add(&hm, &key1, &val1);
    HashMap_Add(&hm, &key2, &val2);
    HashMap_Add(&hm, &key3, &val3);
    HashMap_Add(&hm, &key4, &val4);
    HashMap_Add(&hm, &key5, &val5);
    HashMap_Add(&hm, &key6, &val6);

    uint32_t check = HM_GET(uint32_t, &hm, &key2);
    EXPECT_OR_RET(check == val2);
    check = HM_GET(uint32_t, &hm, &key6);
    EXPECT_OR_RET(check == val6);
    string key1clone = StringClone(GetCAllocator(), key1);
    check = HM_GET(uint32_t, &hm, &key1clone);
    EXPECT_OR_RET(check == val1);

    return true;
}
int main()
{
    if (!TestHashMaps()) return 1;

    return 0;
}