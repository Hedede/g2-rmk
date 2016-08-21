//--------------------------------------
// Kamera
//--------------------------------------

const int NUM_FRUSTUM_PLANES = 6;

class zCCamera {
public:
	void SetFOV(float fov)
	{
		double aspect = xdim ? double(ydim) / double(xdim) : 0.5625;
		SetFOV(fov, aspect * fov);
	}
private:
    //enum { CLIP_FLAGS_FULL        = 63, CLIP_FLAGS_FULL_WO_FAR    = 15 };
    //enum { NUM_FRUSTUM_PLANES_WO_FAR  =  4 };
    //enum { FRUSTUM_PLANE_FAR  =  4 };

    //           frustumplanes   [NUM_FRUSTUM_PLANES];
	zTPlane frustumplanes[NUM_FRUSTUM_PLANES];
        int signbits[/*NUM_FRUSTUM_PLANES als Bytes*/ 2];   //zBYTE

    struct zTViewportData {
        int xmin;               //int           // oben rechts
        int ymin;
        int xdim;
        int ydim;               //int         
        float xminFloat;          //zVALUE        // oben links
        float yminFloat;          //zVALUE    
        float xmaxFloat;          //zVALUE        // unten rechts
        float ymaxFloat;          //zVALUE    
        float xdimFloat;          //zVALUE        
        float ydimFloat;          //zVALUE    
        float xdimFloatMinus1;    //zVALUE    
        float ydimFloatMinus1;    //zVALUE    
        float xcenter;            //zVALUE
        float ycenter;            //zVALUE
    } vpData;
    
    int targetView;                         //zCViewBase

    //Transformationsmatrizzen:
    zMATRIX4 camMatrix   ;       //
    zMATRIX4 camMatrixInv;       //

    zBOOL  tremorToggle;       //
    zREAL  tremorScale;        //
    zVEC3  tremorAmplitude; //
    zVEC3  tremorOrigin;    //
    zREAL  tremorVelo;         //

    // Transformation matrices
    zMATRIX4  trafoView   ;   //
    zMATRIX4  trafoViewInv;   //
    zMATRIX4  trafoWorld  ;   //
    
    /*
    template <class T, int SIZE> class zCMatrixStack {
        int     pos;
        T       stack[SIZE];
    }*/
    //zCMatrixStack<zMATRIX4,8> trafoViewStack;
        int trafoViewStack[/* 1 + 16 * 8 */129];
    //zCMatrixStack<zMATRIX4,8> trafoWorldStack;
        int trafoWorldStack[/* 1 + 16 * 8 */129];
    //zCMatrixStack<zMATRIX4,8> trafoWorldViewStack;
        int trafoWorldViewStack[/* 1 + 16 * 8 */129];
    
    zMATRIX4 trafoProjection; //

    //enum { zTCAM_POLY_NUM_VERT = 4 };
    
    /*
    struct zTCamVertSimple {
        zREAL       x,y,z;      
        zVEC2       texuv;
        zCOLOR      color;
    };
    */
    
    //zTCamVertSimple polyCamVerts[zTCAM_POLY_NUM_VERT];
        int polyCamVerts[/*zTCAM_POLY_NUM_VERT * (3 + 2 + 1)*/ 24];
    
    zCPolygon*   poly;           //
    zCMesh*      polyMesh;       //
    zCMaterial*  polyMaterial;   //

    // Screen-Effects
    zBOOL   screenFadeEnabled;         //
    zCOLOR  screenFadeColor;           //
    zSTRING screenFadeTexture;         //
    zREAL   screenFadeTextureAniFPS;   //
    
    /*
    enum zTRnd_AlphaBlendFunc   {   zRND_ALPHA_FUNC_MAT_DEFAULT,
                                zRND_ALPHA_FUNC_NONE,                   
                                zRND_ALPHA_FUNC_BLEND,              
                                zRND_ALPHA_FUNC_ADD,                    
                                zRND_ALPHA_FUNC_SUB,                    
                                zRND_ALPHA_FUNC_MUL,                    
                                zRND_ALPHA_FUNC_MUL2,                   
                                zRND_ALPHA_FUNC_TEST,   
                                zRND_ALPHA_FUNC_BLEND_TEST
                            };  */
    int screenFadeTextureBlendFunc;     //zTRnd_AlphaBlendFunc
    zBOOL cinemaScopeEnabled;             //zBOOL 
    zCOLOR cinemaScopeColor;               //zCOLOR

    //ungenutzt:
    //enum zPROJECTION  { PERSPECTIVE, ORTHOGONAL };
    int projection;     //zPROJECTION
    
    /*
    enum zTCam_DrawMode { zCAM_DRAW_NORMAL, zCAM_DRAW_NOTHING, 
                      zCAM_DRAW_WIRE, zCAM_DRAW_FLAT, zCAM_DRAW_TEX }; */
    int drawMode; //zTCam_DrawMode
    
    /*
    enum zTShadeMode    { zSHADE_NORMAL, zSHADE_NOTHING, zSHADE_CONSTANT, zSHADE_GOURAUD, zSHADE_LIGHTMAP }; */
    int shadeMode;  //zTShadeMode
    
    zBOOL drawWire;   //zBOOL

    zVALUE    farClipZ;          //
    zVALUE    nearClipZ;         //
    zVALUE    viewDistanceX;     //
    zVALUE    viewDistanceY;     //
    zVALUE    viewDistanceXInv;  //
    zVALUE    viewDistanceYInv;  //
    zBOOL     vobFarClipZ;       //
    zREAL     fovH;              //
    zREAL     fovV;              //
    zCVob*    connectedVob;      //
             
    zVALUE    topBottomSin;      //
    zVALUE    topBottomCos;      //
    zVALUE    leftRightSin;      //
    zVALUE    leftRightCos;      //
};
