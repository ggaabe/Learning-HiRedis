#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <hiredis/hiredis.h>

#include "NumberGenerator.h"

/**
 * Connects to Redis, passes that connection to integer generator
 * the generator, and returns whatever the generator returns.
 * 
 */
int main(int argc, char const *argv[])
{
	redisContext *context = malloc(sizeof(redisContext));
	printf("%s\n", "Now setting up Database");
	setupDB(context);
	printf("%s\n", "Now generating numbers");
	return generate(context);
}

/**
 * Takes a pointer to the redis context (which represents a connection)
 * populates it with a function call that connects to the database, and 
 * then prints out things if there is an error.
 * 
 * Code is taken from https://github.com/redis/hiredis/blob/master/examples/example.c
 */
void setupDB(redisContext *newContext)
{
	struct timeval timeout = { 1, 500000 };
	newContext = redisConnectWithTimeout(
		"localhost", 
		6379,
		timeout);
	if (newContext == NULL || newContext->err) {
		if (newContext) {
			printf("Connection error: %s\n", newContext->errstr);
			redisFree(newContext);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
	}
}

/**
 * Increments an integer from one to a thousand with random delays, and 
 * calls a function to update the key in the redis database every time.
 * 
 */
int generate(redisContext *newContext)
{
	for (int i = 0; i < 1000; ++i)
	{
		printf("%s %d\n", "Generated a new number, i is now", i);
		updateDB(newContext, i);
		usleep(400 * 1000 * (rand() % 20));
	}
	return 0;
}

/**
 * updates the "NumberGeneratorValue" key in the database, using the 
 * pointer to the 'context' connection struct.
 * 
 * uses %b and WTF
 */
char* updateDB(redisContext *newContext, int newValue)
{
	redisReply *reply;
	printf("%s\n", "got to here (updateDB)");
	int newStrValueLength = intStrLen(newValue);
	char newStrValue[newStrValueLength];
	printf("%s\n", "got to there (updateDB)");
	sprintf(newStrValue, "%d", newValue);

	printf("%s %d %s %s%s\n", "The value", newValue, "was converted into the string", newStrValue, ". Running redisCommand.");

	reply = redisCommand(
		newContext,
		"SET NumberGeneratorValue %b",
		newStrValue,
		(size_t) newStrValueLength);

	printf("%s\n", "Redis command returned reply.");

	char* result = reply->str;
	freeReplyObject(reply);
	printf("%s %s\n", "reply from server was", result);
	return result;
}

/**
 * returns the length of the string representation of the integer.
 */
int intStrLen(int a) {
	int result = 1;
	printf("a is initially %d, res is %d\n", a, result);
	for(;!(a >= 0 && a <= 9);) {
		printf("a is %d, res is %d\n", a, result);
		result++;
		a /= 10;
	}
	printf("%s %d\n", "intStrLen returning", result);
	return result;
}
