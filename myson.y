%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);
FILE* in;
FILE * out;
int max_length = 200;
//////////////////////////////////////////////////////////////////////////////////////////
// Code from calc.c (the maing bignum code)
///////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	int *digits;        
	int sign;			/* 1 if positive, -1 if negative */ 
	int decimal_point;			/* when no. is written in reverse index of digit *after* decimal point( 85.4 -> 1, 9988.23 ->2, 1.45345 -> 5 ()*/
	int last_digit;   /* index of last _dig(first digit in reality) = length of no.- 1 */
} bignum;
int find_last_index(bignum *a);
bignum* add_bignum_proto(bignum *a, bignum *b);
bignum* product_with_digit(bignum *inp, int dig);
bignum* product(bignum *a, bignum *b, int also_round_ones);
void shift_right(bignum *inp, int k);
void shift_left(bignum *inp, int k);
int divide_guess_digit(bignum* a, bignum* b);
bignum* divide(bignum *a, bignum *b, int also_round_ones);
bignum* divide_single_digit(bignum* a, int b);
bignum* bigify_int(int inp);
bignum* square_root(bignum *a);
bignum* logarithm(bignum  *inp);
bignum*  power(bignum *a, bignum *b);
char* bignum_to_string(bignum *inp);
bignum* string_to_bignum(char *inp);
void add_bignum_positive(bignum *a, bignum *b, bignum *c);
bignum* add_bignum(bignum *a, bignum *b, int also_round_ones);
bignum* complement(bignum *a);
bignum* subtract_bignum(bignum *a, bignum *b, int also_round_ones);
void align_decimals(bignum *a, int shift_amount);
void remove_leading_zeroes(bignum *c);
int is_greater(bignum *a, bignum*b);
void subtract_bignum_positive(bignum* a, bignum* b,bignum* c);
bignum* negate(bignum* inp);
bignum* round_mine(bignum* inp, int also_round_ones);
void shift_left(bignum *inp, int k){ // NOTE : Unsafe since it changes the passed thingy
	int a,b;  // decimal point not  taken care of
			// overwrites 0th index
	for(b =0;b<k;b++){
		for(a=0;a<inp->last_digit;a++){
			inp->digits[a] = inp->digits[a+1];
		}
		inp->digits[inp->last_digit] = 0;
	}
	inp->last_digit = find_last_index(inp);
}

bignum* round_mine(bignum* inp, int also_round_ones){

	// round_mine from 2* max_len to max_len 
	// give error if its integral value can' fit
	bignum *ret=  malloc(sizeof *ret);
	ret->digits = (int *) malloc((max_length*2) * sizeof(int)) ; 
	int kk;
	ret->sign = inp->sign;
	ret->last_digit = inp->last_digit;
	ret->decimal_point = inp->decimal_point;
	for(kk=0;kk<max_length*2;kk++){
		ret->digits[kk] = inp->digits[kk];
	}
	int temp_last_index;
	temp_last_index = ret->last_digit;
	while(temp_last_index >= max_length){
		if(ret->decimal_point>0){ // if ret.dig[0] is decimal
			shift_left(ret,1);
			ret->decimal_point -= 1;
			temp_last_index = find_last_index(ret);
		}
		else{
			if(also_round_ones){
				shift_left(ret,1);
				//ret->decimal_point -= 1;
				temp_last_index = find_last_index(ret);
			}
			else{
				//error
				fprintf(out,"LowPrec");
				//fclose(out);
				//fclose(in);
				exit(1);
			}
		}
	}
	return ret;
}

bignum* negate(bignum* inp){
	bignum *ret =  add_bignum(inp,bigify_int(0),0);
	ret->sign = -inp->sign;
	return ret;
}
void initialize_dig_to_zero(bignum *inp){
	int kk ;
	for(kk = max_length*2 -1 ; kk>=0 ;kk--){
		inp->digits[kk] = 0;
	}
}
bignum* product_with_digit(bignum *inp, int dig){
	int carry=0;
	int kk;
	//int res[max_length*2];
	bignum *ret=  malloc(sizeof *ret);
	ret->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	initialize_dig_to_zero(ret);
	for(kk=0;kk <= inp->last_digit;kk++){
		ret->digits[kk] = (inp->digits[kk]*dig +carry)%10;
		carry= (inp->digits[kk]*dig +carry)/10;
	}
	// round_mine off and move res to ret
	ret->digits[inp->last_digit+1] = carry;
	ret->decimal_point = inp->decimal_point;
	ret->last_digit = find_last_index(ret);
	ret->sign = 1;
	return ret;
}
bignum* product(bignum *a, bignum *b, int also_round_ones){
	bignum *ret=  malloc(sizeof *ret);
	ret->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	initialize_dig_to_zero(ret);
	ret->decimal_point=0;
	ret->sign = 1;
	int k;
	bignum *resp;
	bignum *sresp;
	for(k=0;k<=b->last_digit;k++){
		resp = product_with_digit(a,b->digits[k]);
		resp->decimal_point=0; // no decimal addition wanted
		shift_right(resp,k);
		sresp = add_bignum(resp,ret,1);
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
	//round_mine  
	return round_mine(ret,0);
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
		if(is_greater(guess_product,a)==1){
			free(guess_product);
			continue;
		}
		else{
			free(guess_product);
			break;
		}
	}
	if(kk==-1){
		return 0;
	}
	return kk;
}
bignum* divide(bignum *a, bignum *b, int also_round_ones){
	// ignore decimals in inputs initially, taking care of them later 
	// use is_greater for compare 1 if a>b -1 otherwise
	// I store dec points of inputs at start and cange them, will change back at end,
	int checkz;
	checkz = find_last_index(b);
	if(checkz==-1){
		fprintf(out, "DivErr");
		//fclose(out);
		//fclose(in);
		exit(1);
	}
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
			a = product(a,bigify_int(10),0);
		}
	}
	a->decimal_point = 0 ;
	b->decimal_point = 0 ;
	a->sign = 1 ;
	b->sign = 1 ;
	bignum* ret = malloc(sizeof *ret);
	ret->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	initialize_dig_to_zero(ret);
	bignum* dividend = malloc(sizeof *dividend);
	dividend->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	initialize_dig_to_zero(dividend);
	bignum* guess_product = malloc(sizeof *guess_product);
	guess_product->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	
	initialize_dig_to_zero(guess_product);

	bignum* subtracted = malloc(sizeof *subtracted);
	subtracted->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	initialize_dig_to_zero(subtracted);
	ret->last_digit = find_last_index(ret); //not needed probz if sub also does this
	ret->sign = 1;
	ret->decimal_point = 0;
	dividend->sign = 1;
	dividend->last_digit = 0;
	dividend->decimal_point = 0;
	int taken_digit = a->last_digit;
	dividend->digits[0] = a->digits[a->last_digit];
	//dividend = product_with_digit(a,1);
	int guess;
	int flaggy = 0;
	while( (dividend->last_digit >= 0) &&(ret->last_digit < (2*max_length - 1))  )  { //find last dig gives -1 for number 0 
		guess = divide_guess_digit(dividend,b);
		guess_product  = product_with_digit(b,guess);
		subtracted = subtract_bignum(dividend,guess_product,1);
		free(dividend);
		subtracted->sign = 1;
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
			if(is_greater(dividend,bigify_int(0))==1){
				shift_right(dividend,1);
				dividend->digits[0] = 0;
				ret->decimal_point += 1;
				
				flaggy = 1;
			}
		}
		dividend->last_digit = find_last_index(dividend); //not needed probz if sub also does this
		ret->last_digit = find_last_index(ret); //not needed probz if sub also does this
		free(guess_product);

	}
	if(ret->last_digit>= 2*max_length-1){
		ret->decimal_point -= 1;
	}
	free(dividend);
	a->decimal_point = deca ;
	b->decimal_point = decb ;
	a->sign = signa ;
	b->sign = signb ;
	if(decimalpoints>=0){
		if(ret->decimal_point== (1+max_length*2)){
			ret->decimal_point --;
		}
		ret->decimal_point += decimalpoints;
		remove_leading_zeroes(ret);
	}
	if(a->sign==b->sign){
		ret->sign = 1;
	}
	else{
		ret->sign  = -1;
	}
	return round_mine(ret,also_round_ones);
}

bignum* bigify_int(int inp){
	bignum *temp;
	temp = malloc(sizeof *temp);
	temp->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	initialize_dig_to_zero(temp);

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


int find_last_index ( bignum *inp ) { 
	int c=0; //  returns -1 if no digits in no. ie number is 0
	int flag=0;
	int kk;
	for(kk=2*max_length-1;kk>=0;kk--){
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
	ret->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	initialize_dig_to_zero(ret);
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
		ret = concat(ret,"- ");
	}
	int try;
	if(inp->decimal_point>inp->last_digit){
		try = inp->decimal_point;
	}
	else{
		try = inp->last_digit;
	}
	for(kk = try; kk>=0;kk--){
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



//not worried about round_mineing off since printing will take care of them

void add_bignum_positive(bignum *a, bignum *b, bignum *c) { //c = a+ b all positive
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
		c->decimal_point = dec2;
	}
	else{
		flag=2;
		align_decimals(b, d-e);
		c->decimal_point = dec1;
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

bignum* add_bignum(bignum *a, bignum *b, int also_round_ones) {
	bignum*c = (bignum *)malloc(sizeof(bignum)); //does c = a+b
	c->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

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
	c->last_digit = find_last_index(c);
	a->last_digit = find_last_index(a);
	b->last_digit = find_last_index(b);
	return round_mine(c,also_round_ones);
}


void subtract_bignum_positive(bignum* a, bignum* b,bignum* c){// c = a-b a>b, all positive
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
	else
		c->decimal_point = dec1; //any of the two, doesn't matter
	 //SET C's DECIMAL IF NOT SET 
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
	c->last_digit = find_last_index(c);

}


int is_greater(bignum *a, bignum*b){ //checks if a>b if yes then 1 else -1 
	if(a->sign>0 && b->sign<0)
		return 1;
	if(a->sign<0 && b->sign>0)
		return -1;
	if(a->sign>0 && b->sign>0){
			int dec1 = a->decimal_point;
			int dec2 = b->decimal_point;
			int c = (dec2<dec1) ? dec2 : dec1;
			int d = (dec2<dec1) ? dec1 : dec2;
			int flag;
			if(dec1<dec2) 
				align_decimals(a, d-c);
			else
				align_decimals(b, d-c);
			int k = (a->last_digit > b->last_digit)? a->last_digit : b->last_digit;
			for(int i= k; i>=0; i--){
				if(a->digits[i]>b->digits[i])
					return 1;
				if(a->digits[i]<b->digits[i])
					return -1;
			}
			remove_leading_zeroes(a);
			remove_leading_zeroes(b);
			return -1;
			
		
	if(a->sign<0 && b->sign<0){
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

bignum* subtract_bignum(bignum *a, bignum *b, int also_round_ones) {
	bignum *c = (bignum *)malloc(sizeof(bignum)); //Does c=a-b
	c->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

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
	
	c->last_digit=find_last_index(c);
	a->last_digit=find_last_index(a);
	b->last_digit=find_last_index(b);
	return round_mine(c,also_round_ones);
}

void align_decimals(bignum *inp, int k){ // NOTE : Unsafe since it changes the passed thingy
	int a,b;  // decimal point not  taken care of
	for(b =0;b<k;b++){
		for(a=inp->last_digit;a>=0;a--){
			inp->digits[a+1] = inp->digits[a];
		}
		inp->last_digit += 1;
		inp->digits[0] = 0;
	}
	inp->decimal_point+= k;
	inp->last_digit+=k;
}

void remove_leading_zeroes(bignum *inp) {
	int p=0;
	while(inp->digits[p]==0 && p<inp->decimal_point)
		p++;
	//printf("p=%d\n", p);
	int a,b;  // decimal point not  taken care of
			// overwrites 0th index
	for(b =0;b<p;b++){
		for(a=0;a<inp->last_digit;a++){
			inp->digits[a] = inp->digits[a+1];
		}
		inp->digits[inp->last_digit] = 0;
	}
	inp->last_digit -=p;
	inp->decimal_point-=p;
}


bignum* integerpower(bignum *a, int pow){ //b->int
	int i=0;
	bignum* temp = (bignum *)malloc(sizeof(bignum));

	temp->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	temp = bigify_int(1);

	for(i=0; i<pow; i++){
		temp = product(a,temp,0);
	}
	if(a->sign>0)
		temp->sign=1;
	else
	{
		if(pow%2==0)
			temp->sign =1;
		else
			temp->sign = -1;
	}
	return temp;
}

bignum* epowerX(bignum *a) {
	bignum* temp = bigify_int(1);
	bignum* fact = bigify_int(1);
	bignum* one = bigify_int(1);
	bignum* counter = bigify_int(1);
	bignum* epsilon = (bignum *) malloc(sizeof(bignum));
	epsilon->digits = (int *) malloc(2*max_length*sizeof(int));
	epsilon->digits[0] = 1;
	epsilon->decimal_point = 4;
	epsilon->sign = 1;
	epsilon->last_digit = 0;

	for(int i=1; ; i++){
		bignum* term = divide(integerpower(a,i), fact,0);
		if(is_greater(term, epsilon) == -1){
			break;
		}
		temp = add_bignum(temp, term,0);
		counter = add_bignum(counter, one,0);
		fact = product(fact,counter,0);
	}
	return temp;
}

bignum* logoneplusX(bignum *a){
	bignum* temp = bigify_int(0);
	bignum* fact = bigify_int(1);
	bignum* one = bigify_int(1);

	for(int i=1; i<9; i++){
		if(i%2==1){
			temp = add_bignum(temp, divide(integerpower(a,i), fact,0),0);
		}
		else{
			temp = subtract_bignum(temp, divide(integerpower(a,i), fact,0),0);
		}
		fact = product(fact,add_bignum(fact, one,0),0);
	}
	return temp;
}
bignum* square_root_mm(bignum *a){
	if(a->sign<0){
		fprintf(out, "SqrtErr");
		//fclose(out);
		//fclose(in);
		exit(1);
	}
	bignum* pro;
	bignum* temp;
	bignum* base10 = (bignum *)malloc(sizeof(bignum));
	base10->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	base10->sign = 1;
	base10->last_digit = 0;
	base10->digits[0]=1;
	base10->digits[1]=0;
	base10->digits[2]=0;
	base10->digits[3]=0;
	base10->decimal_point = 4;
	bignum* copy = (bignum *)malloc(sizeof(bignum));
	copy->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	copy->sign = 1;
	copy->last_digit = 0;
	copy->digits[0]=1;
	copy->digits[1]=0;
	copy->digits[2]=0;
	copy->digits[3]=0;
	copy->decimal_point = 4;
	pro = product(copy,copy,0);
	while ( is_greater(a, pro)==1){
		free(pro);
		pro = product(copy,copy,0);
		temp = copy;
		copy = add_bignum(copy,base10,0);
		free(temp);
	}
	return round_mine(copy,0);
}

bignum* logarithm(bignum *a){
	int length = a->last_digit - a->decimal_point ;
	int dec = a->decimal_point;

	bignum* base10 = (bignum *)malloc(sizeof(bignum));
	base10->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	base10->sign = 1;
	base10->last_digit = 3;
	base10->digits[0]=3;
	base10->digits[1]=0;
	base10->digits[2]=3;
	base10->digits[3]=2;
	base10->decimal_point = 3;


	if(length<0){
		length = -length -1;
		a->decimal_point-=length;
	}
	else{
		length = length + 1;
		a->decimal_point+=length;
	}
	bignum* first;
	//first->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	first = divide(logoneplusX(subtract_bignum(a,bigify_int(1),0)), base10,0);
	if(length>=0){
		first = add_bignum(first, bigify_int(length),0);
	}
	else{
		first = subtract_bignum(first, bigify_int(length),0);
	}
	a->decimal_point =dec;
	a->last_digit = find_last_index(a);

	
	//return first;
	return first;
}

bignum* power(bignum *a, bignum *b){
	if(b->decimal_point ==0)
		return integerpower(a, atoi(bignum_to_string(b)));
	bignum* base10 = (bignum *)malloc(sizeof(bignum));
	base10->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 
	base10->sign = 1;
	base10->last_digit = 3;
	base10->digits[0]=3;
	base10->digits[1]=0;
	base10->digits[2]=3;
	base10->digits[3]=2;
	base10->decimal_point = 3;
	return epowerX(product(b,divide(logarithm(a), base10,0),0));// assume a>0 ; b can be positive or negative
}

bignum* square_root(bignum *a){
	bignum*b = (bignum *)malloc(sizeof(bignum));
	b->digits = (int *) malloc(max_length*2 * sizeof(int)) ; 

	b->digits[0]=5;
	b->decimal_point=1;
	b->sign=1;
	b->last_digit = 0;

	if(a->sign<0){
		fprintf(out, "SqrtErr");
		//fclose(out);
		//fclose(in);
		exit(1);
	}
	else
		return power(a,b);
}
/////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////
// Code from calc.c END(the maing bignum code)
///////////////////////////////////////////////////////////////////////////////////////
%}

%union {
	char* ival;
}

%token<ival> T_INT
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_LEFT T_RIGHT T_SQRT T_LOG T_POW T_COMMA
%token T_NEWLINE T_QUIT
%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE 
%right T_SQRT T_LOG T_POW 


%left T_RIGHT
%right T_LEFT
%right U_MINUS

%type<ival> expression

%start calculation

%%

calculation: 
	   | calculation line
;

line: T_NEWLINE
    | expression T_NEWLINE {  fprintf(out,"%s\n", $1); /* write convert bignum to string here */ } 
    | T_QUIT T_NEWLINE { fprintf(out,"bye!\n"); exit(0); }
;
expression: T_INT				
	  | expression T_PLUS expression	{ $$ = bignum_to_string(add_bignum(round_mine(string_to_bignum($1),0),round_mine(string_to_bignum($3),0),0));}
	  | expression T_MINUS expression	{ $$ = bignum_to_string(subtract_bignum(round_mine(string_to_bignum($1),0),round_mine(string_to_bignum($3),0),0));}
	  | expression T_MULTIPLY expression	{ $$ = bignum_to_string(product(round_mine(string_to_bignum($1),0),round_mine(string_to_bignum($3),0),0));}
	  | expression T_DIVIDE expression	{ $$ = bignum_to_string(divide(round_mine(string_to_bignum($1),0),round_mine(string_to_bignum($3),0),0));}
	  | T_LEFT expression T_RIGHT		{ $$ = $2;}
	  | T_MINUS expression 	%prec U_MINUS	{ $$ = bignum_to_string(negate(round_mine(string_to_bignum($2),0)));}
	  | T_SQRT expression				{ $$ = bignum_to_string(square_root_mm(round_mine(string_to_bignum($2),0)));}
	  | T_LOG  expression   			{ $$ = bignum_to_string(logarithm(round_mine(string_to_bignum($2),0)));}
	  | T_POW T_LEFT expression T_COMMA expression T_RIGHT  { $$ = bignum_to_string(power(round_mine(string_to_bignum($3),0),round_mine(string_to_bignum($5),0)));}

%%

int main (int argc, char *argv[]) {
	if(argc<3){
	yyin = stdin;
	out = stdout;
	}
	else{
		in = fopen(argv[1],"r");
		out = fopen(argv[2],"w");
		fscanf(in, "%d", &max_length);
		yyin = in;
	}
	do {
		yyparse();
	} while(!feof(yyin));
	//fclose(out);
	//fclose(in);
	return 0;
}
void yyerror(const char* s) {
	fprintf(out, "SynErr");
//	fclose(out);
//	fclose(in);
	exit(1);
}