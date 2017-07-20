////////////////////////////////////////////////////////////
/// 头文件包含
////////////////////////////////////////////////////////////
#include <BGE/System/Math.h>

namespace bge
{

//! 对给定点使用4*3矩阵处理,获取4*4矩阵
void Math::transform(const float pos[3],const float r[12],float matrix[16])
{
    matrix[0] =r[0];
    matrix[1] =r[4];
    matrix[2] =r[8];
    matrix[3] =0;
    matrix[4] =r[1];
    matrix[5] =r[5];
    matrix[6] =r[9];
    matrix[7] =0;
    matrix[8] =r[2];
    matrix[9] =r[6];
    matrix[10]=r[10];
    matrix[11]=0;
    matrix[12]=pos[0];
    matrix[13]=pos[1];
    matrix[14]=pos[2];
    matrix[15]=1;
    //! glPushMatrix();
    //! glMultMatrixf(matrix);
}

Vector3f Math::bezier(Vector3f *p,int n,float t)
{
    int k,kn,nn,nkn;
    float blend,muk,munk;
    Vector3f b;

    if(abs(t-1)<1e-6)
    {
	    b.x_ = p[n].x_;
	    b.y_ = p[n].y_;
	    b.z_ = p[n].z_;
	    return(b);
    }

    muk = 1;
    munk = pow(1-t,(float)n);

    for(k=0;k<=n;k++)
	{
        nn = n;
        kn = k;
        nkn = n - k;
        blend = muk * munk;
        muk *= t;
        munk /= (1-t);
        while(nn >= 1)
		{
            blend *= nn;
            nn--;
            if(kn > 1)
			{
                blend /= (float)kn;
                kn--;
            }
            if(nkn > 1)
			{
                blend /= (float)nkn;
                nkn--;
            }
        }
        b.x_ += p[k].x_ * blend;
        b.y_ += p[k].y_ * blend;
        b.z_ += p[k].z_ * blend;
   }

   return(b);
}

const float Math::PI32 = 3.14159265f;
const float Math::DEGTORAD32 = Math::PI32/180.0f;
const float Math::RADTODEG32 = 180.f/Math::PI32;

}









