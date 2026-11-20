/^[[:space:]]*temp = fopen(rgbValuesPath, "r");$/,/^[[:space:]]*fclose(temp);$/c\
	temp = fopen(rgbValuesPath, "r");\
	if (temp) {\
		if (fscanf(temp, "%d %d %d", &r, &g, &b) != 3) {\
			r = 0; g = 149; b = 135;\
		}\
		fclose(temp);\
	} else {\
		r = 0; g = 149; b = 135;\
	}
/^[[:space:]]*FILE \* firstBootTxt = fopen("system\/settings\/boot.bin", "w");/ ,/^[[:space:]]*fclose(firstBootTxt);$/c\
				FILE * firstBootTxt = fopen("system/settings/boot.bin", "w");\
				if (firstBootTxt) {\
					fprintf(firstBootTxt, "0");\
					fclose(firstBootTxt);\
				}
