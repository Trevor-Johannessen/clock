#include <stdio.h>
#include "letters.h"

int character_width(char c){
	int i, width;
	width = 0;

	for(i=0; letter_blueprint[c].blueprint[i][0] > -1; i++){
		if(letter_blueprint[c].blueprint[i][0] > width)
			width = letter_blueprint[c].blueprint[i][0];
	}
	return width;
}

void character_write(char c, int x, int y){
	int i, p_x, p_y;
	for(i=0; letter_blueprint[c].blueprint[i][0] > -1; i++){
		p_x = letter_blueprint[c].blueprint[i][0]+x;
		p_y = letter_blueprint[c].blueprint[i][1]+y;
		matrix.setPoint(p_y, p_x, !matrix.getPoint(p_y, p_x));
	}
}

void sentence_write(char *str, int x, int y){
	int i, cursor;
	cursor = 0;

	for(i=0;i<strlen(str) && cursor < screen_width;i++){
		character_write(str[i], x+cursor, y);
		if(str[i] == ' ')
			cursor +=1;
		cursor += character_width(str[i])+2;
	}
}

