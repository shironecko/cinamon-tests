#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

char* load_text_file(const char* path) {
	FILE* file = fopen(path, "rb");
	if (!file)
		return 0;

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* contents = (char*)malloc(file_size + 1);
	fread(contents, 1, file_size, file);
	contents[file_size] = 0;
	fclose(file);
	return contents;
}

double calculate_file(char* path, lua_State* lua) {
	char* file_contents = load_text_file(path);
	char* prefix = "result = ";
	char buffer[4096];
	snprintf(buffer, sizeof(buffer), "%s%s", prefix, file_contents);

	int error = luaL_dostring(lua, buffer);
	assert(!error);

	lua_getglobal(lua, "result");
	assert(lua_isnumber(lua, -1));
	double result = lua_tonumber(lua, -1);

	free(file_contents);
	return result;
}

int main() {
	char buff[4096];
	int error;
	lua_State *lua = luaL_newstate();
	luaL_openlibs(lua);

	/* double a = calculate_file("C:/Projects/cinamon/source/test.cn", lua); */
	/* double b = calculate_file("C:/Projects/cinamon/source/test.generated.c", lua); */
	/* printf("a = %f, b = %f\n", (float)a, (float)b); */

	srand(time(0));
	int max_rand_scalar = 9;
	for (int nscalars = 2; nscalars < 30; ++nscalars) {
		for (int iteration = 0; iteration < 40; ++iteration) {
			FILE* out_file = fopen("C:/Projects/cinamon/source/test.cn", "w");
			assert(out_file);

			int scalar = rand() % (max_rand_scalar + 1);
			/* printf("%d", scalar); */
			fprintf(out_file, "%d", scalar);
			for (int iscalar = 1; iscalar < nscalars; ++iscalar) {
				scalar = rand() % (max_rand_scalar + 1);
				char operators[] = "+-*/";
				int ioperator = rand() % (sizeof(operators) / sizeof(*operators) - 1);
				char operator = operators[ioperator];

				/* printf(" %c %d", operator, scalar); */
				fprintf(out_file, " %c %d", operator, scalar);
			}
			
			/* printf("\n"); */
			fclose(out_file);

			int result = system("C:/Projects/cinamon/build/cinamon.exe C:/Projects/cinamon/source/test.cn > C:/Projects/cinamon/source/test.generated.c");
			assert(!result);

			double a = calculate_file("C:/Projects/cinamon/source/test.cn", lua);
			double b = calculate_file("C:/Projects/cinamon/source/test.generated.c", lua);
			double diff = a - b;
			diff = diff < 0 ? -diff : diff;
			if (diff > 0.00000001) {
				char* expression = load_text_file("C:/Projects/cinamon/source/test.cn");
				printf("miss: scalars = %d, a = %f, b = %f, expr = %s\n", nscalars, a, b, expression);
				free(expression);
			}
		}
	}

	lua_close(lua);
	return 0;
}
