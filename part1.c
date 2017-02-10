#include <stdio.h>
#include <stdlib.h>


#define MAX_LENGTH 100

typedef struct {
        int digits[2*MAX_LENGTH];        
	    int sign;			/* 1 if positive, -1 if negative */ 
        int decimal_point;			/* index of high-order digit *///( 85.4 -> 2 (represented as [458]))
        int firstdigit; //stores index of last digit (for e.g 85.4 would be 2 [458] array[2] = 8)
} bignum;

void print(bignum *a);
void add_bignum_positive(bignum *a, bignum *b, bignum *c);
void add_bignum(bignum *a, bignum *b, bignum *c);
bignum* complement(bignum *a);
bignum* complement(bignum *a);
void subtract_bignum(bignum *a, bignum *b, bignum *c);
void align_decimals(bignum *a, int shift_amount);
void remove_leading_zeroes(bignum *c);
int isgreater(bignum *a, bignum*b);
//assume parser returns structs of the type bignum, and makes necessary rounding possible.


void print(bignum *a){
	if(a->sign>0)
		printf("+");
	else
		printf("-");
	for(int i=0 ; i<=a->firstdigit; i++){
		printf("%d",a->digits[i]);
	}
	printf("\n");
}

//not worried about rounding off since printing will take care of them
void add_bignum_positive(bignum *a, bignum *b, bignum *c) {
	int carry = 0;
	int dec1 = a->decimal_point;
	int dec2 = b->decimal_point;
	int e = (dec2<dec1) ? dec2 : dec1;
	int d = (dec2<dec1) ? dec1 : dec2;
	int flag;
	//shift and align decimals
	if(dec1<dec2) {
		flag=1;
		align_decimals(a, d-e);
		c->decimal_point = b->decimal_point;
	}
	else{
		flag=2;
		align_decimals(b, d-e);
		c->decimal_point = b->decimal_point;
	}
	int i;
	for(i=0; i <= ((flag=1) ? (a->firstdigit) : (b->firstdigit)) ; i++){
		c->digits[i] = (a->digits[i] + b->digits[i] + carry)%10;
		carry = (a->digits[i] + b->digits[i] + carry)/10;
	}
	c->digits[i+1] = carry;
	if(c->digits[i+1]==0)
		c->firstdigit=i+1;
	else
		c->firstdigit=i;
	remove_leading_zeroes(c);
	//reset a and b to normal
	if(flag ==1)
		remove_leading_zeroes(a);
	else
		remove_leading_zeroes(b);
}

void add_bignum(bignum *a, bignum *b, bignum *c) { //does c = a+b
	if(a->sign >0 && b->sign>0){
		c->sign = 1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign <0 && b->sign<0){
		c->sign = -1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign >0 && b->sign<0){
		b->sign = 1;
		c->sign = (isgreater(a,b)) ? 1 : -1;
		add_bignum_positive(a,complement(b),c); //doesn't matter that we haven't initialized sign of complement(b), since it isn't used anywhere
		b->sign = -1;
	}

	if(a->sign <0 && b->sign>0){
		a->sign = 1;
		c->sign = (isgreater(a,b)) ? -1 : 1;
		add_bignum_positive(complement(a),b,c);
		a->sign = -1;
	}
}

/*void make_absolute(bignum *a) {
	bignum* c = (bignum *) malloc(sizeof(bignum));
	if(a->sign>0)
		c->sign=1;
	else
		c->sign =-1;
	c->decimal_point = a->decimal_point;
}*/

/*void negate(bignum *a) {
	a->sign = -a->sign;
}*/

bignum* complement(bignum *a){
	//flip bits of a
	for(int i=0; i<= a->firstdigit; i++){
		a->digits[i] = 9- a->digits[i];
	}

	bignum *c = (bignum *) malloc(sizeof(bignum));

	//initialize bignum b -> +1;
	bignum *b = (bignum *) malloc(sizeof(bignum));	
	b->decimal_point = 0;
	b->digits[0] = 1;
	b->firstdigit = 0;
	b->sign = 1;
	add_bignum_positive(a,b,c);
	return c;
}

int isgreater(bignum *a, bignum*b){ //checks if a>b if yes then 1 else -1 
	if(a->sign>0 && b->sign<0)
		return 1;
	if(a->sign<0 && b->sign>0)
		return -1;
	if(a->sign>0 && b->sign>0){
		if(a->firstdigit - a->decimal_point > b->firstdigit - b->decimal_point)
			return 1;
		else if (a->firstdigit - a->decimal_point < b->firstdigit - b->decimal_point)
			return -1;
		else {
			int dec1 = a->decimal_point;
			int dec2 = b->decimal_point;
			int c = (dec2<dec1) ? dec2 : dec1;
			int d = (dec2<dec1) ? dec1 : dec2;
			int flag;
			if(dec1<dec2) 
				align_decimals(a, d-c);
			else
				align_decimals(b, d-c);
			for(int i= a->firstdigit; i>=0; i--){
				if(a->digits[i]>b->digits[i])
					return 1;
				if(a->digits[i]<b->digits[i])
					return -1;
			}
			return -1;
		}
	}

	if(a->sign<0 && b->sign<0){
		if(a->firstdigit - a->decimal_point > b->firstdigit - b->decimal_point)
			return -1;
		else if (a->firstdigit - a->decimal_point < b->firstdigit - b->decimal_point)
			return 1;
		else {
			int dec1 = a->decimal_point;
			int dec2 = b->decimal_point;
			int c = (dec2<dec1) ? dec2 : dec1;
			int d = (dec2<dec1) ? dec1 : dec2;
			int flag;
			if(dec1<dec2) 
				align_decimals(a, d-c);
			else
				align_decimals(b, d-c);
			for(int i= a->firstdigit; i>=0; i--){
				if(a->digits[i]>b->digits[i])
					return -1;
				if(a->digits[i]<b->digits[i])
					return 1;
			}
			return 1;
		}
	}
}



void subtract_bignum(bignum *a, bignum *b, bignum *c) { //Does c=a-b
	if(a->sign >0 && b->sign<0){
		c->sign = 1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign <0 && b->sign>0){
		c->sign = -1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign >0 && b->sign>0){
		c->sign = (isgreater(a,b)) ? 1 : -1;
		subtract_bignum(a,b,c);
	}

	if(a->sign <0 && b->sign<0){
		a->sign = 1;
		b->sign = 1;
		c->sign = (isgreater(a,b)) ? -1 : 1;
		subtract_bignum(b,a,c);
		a->sign = -1;
		b->sign = -1;
	}
}



void align_decimals(bignum *a, int shift_amount) {
	int temp = a->firstdigit;
	for(int i=0; i<shift_amount; i++){
		a->digits[temp+shift_amount] = a->digits[temp];
		temp--;
	}
	for(int i=0; i<shift_amount; i++)
		a->digits[i]=0;
	a->firstdigit += shift_amount;
	a->decimal_point += shift_amount;

}

void remove_leading_zeroes(bignum *c) {
	int a=0;
	while(c->digits[a]==0 && a<c->decimal_point) {
		a++;
	}
	for(int i=0;i<=(c->firstdigit)-a;i++){
		c->digits[i]=c->digits[i+a];
	}
	c->firstdigit -=a ;
	c->decimal_point -= a;
}

int main() {
	return 1;
}