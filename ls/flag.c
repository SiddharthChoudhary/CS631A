#include<stdio.h>
#include<stdbool.h>
struct Flags{
    bool if_a_Is_True;
    bool if_A_Is_True;
    bool if_c_Is_True;
    bool if_d_Is_True;
    bool if_F_Is_True;
    bool if_f_Is_True;
    bool if_h_Is_True;
    bool if_i_Is_True;
    bool if_k_Is_True;
    bool if_l_Is_True;
    bool if_n_Is_True;
    bool if_q_Is_True;
    bool if_R_Is_True;
    bool if_r_Is_True;
    bool if_S_Is_True;
    bool if_s_Is_True;
    bool if_t_Is_True;
    bool if_u_Is_True;
    bool if_w_Is_True;
};

struct LsComponentForLOption{
    // because remaining fields in a cell like date, permissions, will always have a definite width requirement and will not vary
    int inodeWidth;
    int numberOfLinks;
    int userNameWidth;
    int groupNameWidth;
    int blocksWidth;
    int numberOfBytesWidth;
    int fileNameWidth;
    // char* filename;
};

struct UsernameAndGroupName{
    char* username;
    char* groupname;
};