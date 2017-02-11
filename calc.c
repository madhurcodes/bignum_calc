#include <stdio.h>
#include <stdlib.h>

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
		srep = sum(resp,ret);
		free(ret);
		ret = srep;
		free(resp);
	}
	ret->decimal_point = a->decimal_point + b->decimal_point;
	ret->last_digit = find_last_index(ret);
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

bignum* divide(bignum *a, bignum *b){
	
	
	
	
	
	
	
	
	
	
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
int find_last_index(bignum *inp){
	int c=0;
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

int main(){
	int a = 4;
	//printf("dddd");
	bignum *v=  malloc(sizeof *v);
	
	v->sign =1;
	v->digits[0] = 9;
	v->digits[1] = 6;
	v->digits[2] = 9;
	v->digits[3] = 8;
	v->decimal_point = 2;
	v->last_digit= 3;
	print_big(v);
	printf("\n");
	print_big(product_with_digit(v,8));
	printf("\n");
	
	
	return 0;
}