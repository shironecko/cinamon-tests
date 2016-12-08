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

	srand(time(0));
	int max_rand_scalar = 9;
	for (int nscalars = 3; nscalars < 100; ++nscalars) {
		for (int iteration = 0; iteration < 30; ++iteration) {
            int itry = 0;
            FILE* out_file = 0;
            do {
                out_file = fopen("C:/Users/vsych/Documents/personal_projects/cinamon/source/test.cn", "w");
                ++itry;
            } while (!out_file && itry < 1000);
			assert(out_file);

            int nparens = 0;
            int paren_chance = 40;
            int rparen_chance = 50;
			int scalar = (rand() % max_rand_scalar) + 1;
            if ((rand() % 101) <= paren_chance) {
                fprintf(out_file, "(");
                ++nparens;
            }
			fprintf(out_file, "%d", scalar);
			for (int iscalar = 1; iscalar < nscalars; ++iscalar) {
				scalar = rand() % (max_rand_scalar + 1);
				char operators[] = "+-*/";
				int ioperator = rand() % (sizeof(operators) / sizeof(*operators) - 1);
				char operator = operators[ioperator];

                char* lparen_str = "";
                char* rparen_str = "";
                if ((rand() % 101) <= paren_chance) {
                    if (nparens && (rand() % 101) <= rparen_chance) {
                        rparen_str = ")";
                        --nparens;
                    } else {
                        lparen_str = "(";
                        ++nparens;
                    }
                }
				fprintf(out_file, " %c %s%d%s", operator, lparen_str, scalar, rparen_str);
			}

            for (int i = 0; i < nparens; ++i) {
                fprintf(out_file, ")");
            }
			
			fclose(out_file);

			int result = system("C:/Users/vsych/Documents/personal_projects/cinamon/build/cinamon.exe C:/Users/vsych/Documents/personal_projects/cinamon/source/test.cn > C:/Users/vsych/Documents/personal_projects/cinamon/source/test.generated.c");
			assert(!result);

			double a = calculate_file("C:/Users/vsych/Documents/personal_projects/cinamon/source/test.cn", lua);
			double b = calculate_file("C:/Users/vsych/Documents/personal_projects/cinamon/source/test.generated.c", lua);
			double diff = a - b;
			diff = diff < 0 ? -diff : diff;
			if (diff > 0.00000001) {
				char* expression = load_text_file("C:/Users/vsych/Documents/personal_projects/cinamon/source/test.cn");
				printf("miss: scalars = %d, a = %f, b = %f, expr = %s\n", nscalars, a, b, expression);
				free(expression);
			}
		}
	}

    printf("END\n");
	getchar();
	lua_close(lua);
	return 0;
}
