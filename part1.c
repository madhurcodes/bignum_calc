 #include <stdio.h>
#include <stdlib.h>

//add and subtract functions dont return bignum*, just initialize a bignum* first and then use these
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
void subtract_bignum(bignum *a, bignum *b, bignum *c);
void align_decimals(bignum *a, int shift_amount);
void remove_leading_zeroes(bignum *c);
int isgreater(bignum *a, bignum*b);
void subtract_bignum_positive(bignum* a, bignum* b,bignum* c);
//assume parser returns structs of the type bignum, and makes necessary rounding possible.


void print(bignum *a){
	if(a->sign>0)
		printf("+");
	else
		printf("-");
	for(int i=a->firstdigit; i>=0; i--){
		printf("%d",a->digits[i]);
	}
	printf("\n");
	//printf("f=%d  sgn=%d d=%d\n", a->firstdigit, a->sign, a->decimal_point);
}

void print_rep(bignum *a){ //prints array representation
	for(int i=0; i<=a->firstdigit; i++){
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
		c->decimal_point = a->decimal_point;
	}
	int i;
	for(i=0; i <= ((a->firstdigit > b->firstdigit) ? (a->firstdigit) : (b->firstdigit)) ; i++) {
		c->digits[i] = (a->digits[i] + b->digits[i] + carry)%10;
		//printf("c, i  = %d,%d\n", c->digits[i],i);
		carry = (a->digits[i] + b->digits[i] + carry)/10;
	}
	c->digits[i] = carry;
	if(c->digits[i]!=0)
		c->firstdigit=i;
	else
		c->firstdigit=i-1;
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
		int temp = isgreater(a,b);
		c->sign = temp;
		if(temp ==1 ){
			subtract_bignum_positive(a,b,c);
		}
		else{
			subtract_bignum_positive(b,a,c);
		}
		 //doesn't matter that we haven't initialized sign of complement(b), since it isn't used anywhere
		b->sign = -1;
	}

	if(a->sign <0 && b->sign>0){
		a->sign = 1;
		int temp = isgreater(a,b);
		c->sign = -temp;
		if(temp ==1 )
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
		a->sign = -1;
	}
	remove_leading_zeroes(c);
}

bignum* complement(bignum *a){
	}

void subtract_bignum_positive(bignum* a, bignum* b,bignum* c){// c = a-b a>b
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
	else if (dec1>dec2){
		flag=2;
		align_decimals(b, d-e);
		c->decimal_point = a->decimal_point;
	}
	int limit = ((a->firstdigit > b->firstdigit) ? a->firstdigit : b->firstdigit);
	for(int i=0; i<= limit; i++) {
		int temp = a->digits[i] - b->digits[i] + carry;
		if (temp<0) {
			c->digits[i] = 10 + temp ;
			carry = -1;
		}
		else{
			c->digits[i] = temp;
			carry = 0;
		}

	}
	c->firstdigit = limit;

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
			remove_leading_zeroes(a);
			remove_leading_zeroes(b);
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
			remove_leading_zeroes(a);
			remove_leading_zeroes(b);
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
		int temp = isgreater(a,b);
		c->sign = temp;
		if(temp==1)
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
		
	}

	if(a->sign <0 && b->sign<0){
		a->sign = 1;
		b->sign = 1;
		int temp = isgreater(a,b);
		c->sign = -temp;
		
		if(temp==1)
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
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
	for(int i=0;i<=c->firstdigit;i++){
		c->digits[i]=c->digits[i+a];
	}
	c->firstdigit -=a ;
	c->decimal_point -= a;
}


int main() {

	bignum *a = (bignum *) malloc(sizeof(bignum));
	int i;
	for(i=0; i<3; i++) {
		a->digits[i] = 3-(i);
	}
	a->firstdigit = 2;
	a->decimal_point=0;
	a->sign = -1;
	print(a);

	bignum *b = (bignum *) malloc(sizeof(bignum));
	b->digits[0]=1;
	b->digits[1] = 1;
	b->firstdigit = 1;
	b->decimal_point=0;
	b->sign = 1;
	bignum *c = (bignum *) malloc(sizeof(bignum));
	print(b);

	add_bignum(a,b,c);
	print(c);
	subtract_bignum(a,b,c);
	print(c);

	return 1;
}

