
int main(int argc, char const *argv[]);

void setupDB(redisContext *newContext);

char* updateDB(redisContext *newContext, int newValue);

int generate(redisContext *newContext);

int intStrLen(int in);
