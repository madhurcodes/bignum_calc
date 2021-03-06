#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max_length 11
typedef struct {
	int digits[max_length*2];        
	int sign;			/* 1 if positive, -1 if negative */ 
	int decimal_point;			/* when no. is written in reverse index of digit *after* decimal point( 85.4 -> 1, 9988.23 ->2, 1.45345 -> 5 ()*/
	int last_digit;   /* index of last _dig(first digit in reality) = length of no.- 1 */
} bignum;
int find_last_index(bignum *a);
bignum* product_with_digit(bignum *inp, int dig);
void print_big(bignum *inp);
bignum* product(bignum *a, bignum *b);
void shift_right(bignum *inp, int k);
int divide_guess_digit(bignum* a, bignum* b);
bignum* divide(bignum *a, bignum *b);
bignum* divide_single_digit(bignum* a, int b);
bignum* bigify_int(int inp);
bignum* square_root(bignum *a);
bignum* logarithm(bignum  *inp);
bignum*  power(bignum *a, bignum *b);
char* bignum_to_string(bignum *inp);
bignum* string_to_bignum(char *inp);
void print(bignum *a);
void add_bignum_positive(bignum *a, bignum *b, bignum *c);
bignum* add_bignum(bignum *a, bignum *b);
bignum* complement(bignum *a);
bignum* subtract_bignum(bignum *a, bignum *b);
void align_decimals(bignum *a, int shift_amount);
void remove_leading_zeroes(bignum *c);
int is_greater(bignum *a, bignum*b);
void subtract_bignum_positive(bignum* a, bignum* b,bignum* c);

bignum* product_with_digit(bignum *inp, int dig){
	int carry=0;
	int kk;
	//int res[max_length*2];
	bignum *ret=  malloc(sizeof *ret);
	for(kk=0;kk <= inp->last_digit;kk++){
		ret->digits[kk] = (inp->digits[kk]*dig +carry)%10;
		carry= (inp->digits[kk]*dig +carry)/10;
	}
	// round off and move res to ret
	ret->digits[inp->last_digit+1] = carry;
	ret->decimal_point = inp->decimal_point;
	ret->last_digit = find_last_index(ret);
	return ret;
}
bignum* product(bignum *a, bignum *b){
	bignum *ret=  malloc(sizeof *ret);
	ret->decimal_point=0;
	int k;
	bignum *resp;
	bignum *sresp;
	for(k=0;k<=b->last_digit;k++){
		resp = product_with_digit(a,b->digits[k]);
		resp->decimal_point=0; // no decimal addition wanted
		shift_right(resp,k);
		sresp = add_bignum(resp,ret);
		free(ret);
		ret = sresp;
		free(resp);
	}
	ret->decimal_point = a->decimal_point + b->decimal_point;
	ret->last_digit = find_last_index(ret);
	if(a->sign==b->sign){
		ret->sign = 1;
	}
	else{
		ret->sign  = -1;
	}
	//round  
	return ret;
}
void shift_right(bignum *inp, int k){ // NOTE : Unsafe since it changes the passed thingy
	int a,b;  // decimal point not  taken care of
	for(b =0;b<k;b++){
		for(a=inp->last_digit;a>=0;a--){
			inp->digits[a+1] = inp->digits[a];
		}
		inp->last_digit += 1;
		inp->digits[0] = 0;
	}
}
int divide_guess_digit(bignum* a, bignum* b){
	int kk;
	bignum* guess_product;
	for(kk=9;kk>=0;kk--){
		guess_product = product_with_digit(b,kk);
		if(is_greater(a,guess_product)){
			free(guess_product);
			continue;
		}
		else{
			free(guess_product);
			break;
		}
	}
	return kk;
}
bignum* divide(bignum *a, bignum *b){
	// ignore decimals in inputs initially, taking care of them later 
	// use is_greater for compare 1 if a>b -1 otherwise
	// I store dec points of inputs at start and cange them, will change back at end,
	int deca, decb, signa,signb;
	deca = a->decimal_point;
	signa = a->sign;
	signb = b->sign;
	decb = b->decimal_point;
	int decimalpoints = deca - decb;
	int kkk;
	if(decimalpoints>=0){
		
	}
	else{
		for(kkk= -decimalpoints;kkk>0 ;kkk--){
			a = product(a,bigify_int(10));
		}
	}
	
	
	
	a->decimal_point = 0 ;
	b->decimal_point = 0 ;
	a->sign = 1 ;
	b->sign = 1 ;
	bignum* ret = malloc(sizeof *ret);
	bignum* dividend = malloc(sizeof *dividend);
	bignum* guess_product = malloc(sizeof *guess_product);
	bignum* subtracted = malloc(sizeof *subtracted);
	dividend->sign = 1;
	dividend->last_digit = 0;
	dividend->decimal_point = 0;
	int taken_digit = a->last_digit;
	dividend->digits[0] = a->digits[a->last_digit];
	//dividend = product_with_digit(a,1);
	int guess;
	while( dividend->last_digit >= 0  )  { //find last dig gives -1 for number 0 
		guess = divide_guess_digit(dividend,b);
		guess_product  = product_with_digit(b,guess);
		subtracted = subtract_bignum(dividend,guess_product);
		free(dividend);
		dividend = subtracted;
		shift_right(ret,1);
		ret->digits[0] = guess;
		if(taken_digit>0){
			//take one more digit
			shift_right(dividend,1);
			taken_digit -=1;
			dividend->digits[0] = a->digits[taken_digit];
		}
		else{
			shift_right(dividend,1);
			dividend->digits[0] = 0;
			ret->decimal_point += 1;
		}
		dividend->last_digit = find_last_index(dividend); //not needed probz if sub also does this
		free(guess_product);

	}
	free(dividend);
	a->decimal_point = deca ;
	b->decimal_point = decb ;
	a->sign = signa ;
	b->sign = signb ;
	if(decimalpoints>=0){
		ret->decimal_point += decimalpoints;
		remove_leading_zeroes(ret);
	}
	if(a->sign==b->sign){
		ret->sign = 1;
	}
	else{
		ret->sign  = -1;
	}
	return ret;
}
bignum* divide_single_digit(bignum* a, int b){
	bignum * ret  ;
	bignum *temp;
	ret = malloc(sizeof *ret);
	temp = bigify_int(b);
	ret = divide(a,temp);
	free(temp);
	return ret;
}
bignum* bigify_int(int inp){
	bignum *temp;
	temp = malloc(sizeof *temp);
	temp->last_digit = -1;
	temp->decimal_point = 0;
	if(inp<0){
		temp->sign=-1;
		inp = -inp;
	}
	else{
		temp->sign=1;
	}
	while(inp>0){
		temp->digits[0] = inp % 10;
		temp->last_digit += 1;
		inp = inp / 10;
	}
	return temp;
}
bignum* square_root(bignum *a){
	bignum* copy;
	bignum* y;
	/* 	bignum* sum;
bignum* div1;
bignum* div2; */
	bignum* accuracy;
	bignum* n;
	accuracy = bigify_int(1);
	accuracy->decimal_point = 6;
	n = add_bignum(a,0);
	copy = product(a,bigify_int(1));
	y = bigify_int(1);
	if(a->sign=-1){

		//raise error and stuff
	}
	else{
		while(is_greater(subtract_bignum(copy,y),accuracy)){  // Im lazy and dont free stuff here (if coz problems then do it)
			copy = divide_single_digit(add_bignum(copy,y),2);
			y = divide(n,copy);
		}
	}
	return copy;
}
///
/*Returns the square root of n. Note that the function */
/* float squareRoot(float n)
{

float x = n;
float y = 1;
float e = 0.000001; 
while(x - y > e)
{
x = (x + y)/2;
y = n/x;
}
return x;
} */

//
bignum* logarithm(bignum  *inp){
	bignum* ret = malloc(sizeof *ret);




	return ret;	
}
bignum*  power(bignum *a, bignum *b){
	bignum* ret = malloc(sizeof *ret);






	return ret;	
}
void print_big(bignum *inp){
	int kk;
	if(inp->sign==-1){
		printf("- ");
	}
	for(kk=inp->last_digit;kk>=0;kk--){
		if(kk==inp->decimal_point-1){
			printf(".");
		}
		printf("%d",inp->digits[kk]);
	}
}
int find_last_index ( bignum *inp ) { 
	int c=0; //  returns -1 if no digits in no. ie number is 0
	int flag=0;
	int kk;
	for(kk=max_length-1;kk>=0;kk--){
		if(flag==0){
			if(inp->digits[kk]==0){
				continue;
			}
			else{
				flag=1;
			}
		}
		c+=1;
	}
	return c-1;
}


bignum* string_to_bignum(char *inp){
	bignum* ret = malloc(sizeof *ret);
	ret->sign = 1;
	int point_seen ;
	int d,a=0;
	if (*inp == '-'){
		inp++;
		ret->sign = -1;
	};
	for (point_seen = 0; *inp; inp++){
		if (*inp == '.'){
			point_seen = 1; 
			continue;
		};
		d = *inp - '0';
		if (d >= 0 && d <= 9){
			if (point_seen) ret->decimal_point+=1;
			shift_right(ret,1);
			ret->digits[a] = d;
		};
	};
	ret->last_digit = find_last_index(ret);
	return ret;
}
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
char* bignum_to_string(bignum *inp){
	char *ret = "";
	int kk;
	if(inp->sign==-1){
		ret = concat(ret,"-");
	}
	for(kk = inp->last_digit; kk>=0;kk--){
		if((kk+1) == inp->decimal_point ){
			ret = concat(ret,".");
		//	ret = malloc(1);
			//*ret = '.';
			//ret++;
		}
		//ret = malloc(1);
		char aa = (char) (48 + inp->digits[kk]);
		char temp[] = "";
		sprintf(temp,"%c",aa);
		ret = concat(ret,temp);
		//ret++;
	}
	return ret;
}



//////////////////////////////////////////////////////////////////
/* bignum* add_bignum(bignum* a , bignum* b){
	char * tee;
	sprintf(tee, "%f", atof(bignum_to_string(a)) + atof(bignum_to_string(b)));
	printf("--%s--",tee);
	return string_to_bignum(tee);
}
bignum* subtract_bignum(bignum* a , bignum* b){
	char * tee;
	sprintf(tee, "%f", atof(bignum_to_string(a)) - atof(bignum_to_string(b)));
	return string_to_bignum(tee);
}
int is_greater(bignum* a , bignum* b){
	if(atof(bignum_to_string(a)) > atof(bignum_to_string(b))){
		return 1;
	}
	
	else{
		return -1;
	}
} */


void print(bignum *a){
	if(a->sign>0)
		printf("+");
	else
		printf("-");
	for(int i=a->last_digit; i>=0; i--){
		printf("%d",a->digits[i]);
	}
	printf("\n");
	//printf("f=%d  sgn=%d d=%d\n", a->last_digit, a->sign, a->decimal_point);
}

void print_rep(bignum *a){ //prints array representation
	for(int i=0; i<=a->last_digit; i++){
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
	for(i=0; i <= ((a->last_digit > b->last_digit) ? (a->last_digit) : (b->last_digit)) ; i++) {
		c->digits[i] = (a->digits[i] + b->digits[i] + carry)%10;
		//printf("c, i  = %d,%d\n", c->digits[i],i);
		carry = (a->digits[i] + b->digits[i] + carry)/10;
	}
	c->digits[i] = carry;
	if(c->digits[i]!=0)
		c->last_digit=i;
	else
		c->last_digit=i-1;
	remove_leading_zeroes(c);
	//reset a and b to normal
	if(flag ==1)
		remove_leading_zeroes(a);
	else
		remove_leading_zeroes(b);
}

bignum* add_bignum(bignum *a, bignum *b) {
	bignum*c = (bignum *)malloc(sizeof(bignum)); //does c = a+b
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
		int temp = is_greater(a,b);
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
		int temp = is_greater(a,b);
		c->sign = -temp;
		if(temp ==1 )
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
		a->sign = -1;
	}
	remove_leading_zeroes(c);
	return c;
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
	int limit = ((a->last_digit > b->last_digit) ? a->last_digit : b->last_digit);
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
	c->last_digit = limit;

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

int is_greater(bignum *a, bignum*b){ //checks if a>b if yes then 1 else -1 
	if(a->sign>0 && b->sign<0)
		return 1;
	if(a->sign<0 && b->sign>0)
		return -1;
	if(a->sign>0 && b->sign>0){
		if(a->last_digit - a->decimal_point > b->last_digit - b->decimal_point)
			return 1;
		else if (a->last_digit - a->decimal_point < b->last_digit - b->decimal_point)
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
			for(int i= a->last_digit; i>=0; i--){
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
		if(a->last_digit - a->decimal_point > b->last_digit - b->decimal_point)
			return -1;
		else if (a->last_digit - a->decimal_point < b->last_digit - b->decimal_point)
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
			for(int i= a->last_digit; i>=0; i--){
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

bignum* subtract_bignum(bignum *a, bignum *b) {
	bignum *c = (bignum *)malloc(sizeof(bignum)); //Does c=a-b
	if(a->sign >0 && b->sign<0){
		c->sign = 1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign <0 && b->sign>0){
		c->sign = -1;
		add_bignum_positive(a,b,c);
	}
	if(a->sign >0 && b->sign>0){
		int temp = is_greater(a,b);
		c->sign = temp;
		if(temp==1)
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
		
	}

	if(a->sign <0 && b->sign<0){
		a->sign = 1;
		b->sign = 1;
		int temp = is_greater(a,b);
		c->sign = -temp;
		
		if(temp==1)
			subtract_bignum_positive(a,b,c);
		else
			subtract_bignum_positive(b,a,c);
		a->sign = -1;
		b->sign = -1;
	}
	return c;
}



void align_decimals(bignum *a, int shift_amount) {
	int temp = a->last_digit;
	for(int i=0; i<shift_amount; i++){
		a->digits[temp+shift_amount] = a->digits[temp];
		temp--;
	}
	for(int i=0; i<shift_amount; i++)
		a->digits[i]=0;
	a->last_digit += shift_amount;
	a->decimal_point += shift_amount;

}

void remove_leading_zeroes(bignum *c) {
	int a=0;
	while(c->digits[a]==0 && a<c->decimal_point) {
		a++;
	}
	for(int i=0;i<=c->last_digit;i++){
		c->digits[i]=c->digits[i+a];
	}
	c->last_digit -=a ;
	c->decimal_point -= a;
}

/////////////////////////////////////////////////////











int main(){
	int a = 4;
	//printf("dddd");
	bignum *v=  malloc(sizeof *v);

	v->sign =1;
	v->digits[0] = 1;
	v->digits[1] = 1;
	//v->digits[2] = 1;
	//v->digits[3] = 8;
	v->decimal_point = 1;
	v->last_digit= 1;
	bignum *t=  malloc(sizeof *t);

	t->sign =1;
	t->digits[0] = 0;
	//t->digits[1] = 1;
	//v->digits[2] = 1;
	//v->digits[3] = 8;
	t->decimal_point = 0;
	t->last_digit= 0;
	print_big(v);
	printf("\n");
	print_big(product_with_digit(v,8));
	printf("%s\n",bignum_to_string(string_to_bignum(bignum_to_string(string_to_bignum(bignum_to_string(v))))));
	print_big(add_bignum(v,t));


	return 0;
}