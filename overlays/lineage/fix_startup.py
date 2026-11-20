from pathlib import Path
import re

utils = Path("CyanogenPSP/include/utils.c")
source = utils.read_text()

safe_int = """int setFileDefaultsInt(char *path, int value, int var)
{
    FILE *temp;
    if (!fileExists(path)) {
        temp = fopen(path, "w");
        if (!temp) return value;
        fprintf(temp, "%d", value);
        fclose(temp);
    }
    temp = fopen(path, "r");
    if (!temp) return value;
    if (fscanf(temp, "%d", &var) != 1) var = value;
    fclose(temp);
    return var;
}"""

safe_float = """float setFileDefaultsFloat(char *path, float value, float var)
{
    FILE *temp;
    if (!fileExists(path)) {
        temp = fopen(path, "w");
        if (!temp) return value;
        fprintf(temp, "%f", value);
        fclose(temp);
    }
    temp = fopen(path, "r");
    if (!temp) return value;
    if (fscanf(temp, "%f", &var) != 1) var = value;
    fclose(temp);
    return var;
}"""

safe_char = """char * setFileDefaultsChar(char path[], char data[], char var[])
{
    FILE *temp;
    if (!fileExists(path)) {
        temp = fopen(path, "w");
        if (!temp) {
            strcpy(var, data);
            return var;
        }
        fprintf(temp, "%s", data);
        fclose(temp);
    }
    temp = fopen(path, "r");
    if (!temp) {
        strcpy(var, data);
        return var;
    }
    if (fscanf(temp, "%255s", var) != 1) strcpy(var, data);
    fclose(temp);
    return var;
}"""

for name, replacement in (
    ("setFileDefaultsInt", safe_int),
    ("setFileDefaultsFloat", safe_float),
    ("setFileDefaultsChar", safe_char),
):
    pattern = re.compile(
        r"(?ms)^(?:int|float|char \*)\s+" + name +
        r"\s*\([^)]*\)\s*\{.*?^\}"
    )
    source, count = pattern.subn(lambda _match: replacement, source, count=1)
    if count != 1:
        raise SystemExit(f"Expected exactly one {name} definition; found {count}")

utils.write_text(source)

main = Path("CyanogenPSP/main.c")
source = main.read_text()
old = '\ttemp = fopen(rgbValuesPath, "r");\n\tfscanf(temp, "%d %d %d", &r, &g, &b);\n\tfclose(temp);'
new = '\ttemp = fopen(rgbValuesPath, "r");\n\tif (temp) {\n\t\tif (fscanf(temp, "%d %d %d", &r, &g, &b) != 3) {\n\t\t\tr = 0; g = 149; b = 135;\n\t\t}\n\t\tfclose(temp);\n\t} else {\n\t\tr = 0; g = 149; b = 135;\n\t}'
if old not in source:
    raise SystemExit("Could not find RGB settings read block")
source = source.replace(old, new, 1)

old = '\t\t\t\tFILE * firstBootTxt = fopen("system/settings/boot.bin", "w"); \n\t\t\t\tfprintf(firstBootTxt, "0");\n\t\t\t\tfclose(firstBootTxt);'
new = '\t\t\t\tFILE * firstBootTxt = fopen("system/settings/boot.bin", "w");\n\t\t\t\tif (firstBootTxt) {\n\t\t\t\t\tfprintf(firstBootTxt, "0");\n\t\t\t\t\tfclose(firstBootTxt);\n\t\t\t\t}'
if old in source:
    source = source.replace(old, new, 1)

main.write_text(source)
print("Startup file-access guards applied.")
