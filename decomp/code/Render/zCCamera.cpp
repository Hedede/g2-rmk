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
	enum {
		CLIP_FLAGS_FULL        = 63,
		CLIP_FLAGS_FULL_WO_FAR    = 15 };
	enum { NUM_FRUSTUM_PLANES_WO_FAR  =  4 };
	enum { FRUSTUM_PLANE_FAR  =  4 };

	zTPlane frustumplanes[NUM_FRUSTUM_PLANES];
	zBYTE signbits[NUM_FRUSTUM_PLANES];

	struct zTViewportData {
		int xmin;     // oben rechts
		int ymin;
		int xdim;
		int ydim;
		float xminFloat;          // oben links
		float yminFloat;
		float xmaxFloat;          // unten rechts
		float ymaxFloat;
		float xdimFloat;
		float ydimFloat;
		float xdimFloatMinus1;
		float ydimFloatMinus1;
		float xcenter;
		float ycenter;
	} vpData;

	zCViewBase* targetView;

	//Transformationsmatrizzen:
	zMATRIX4 camMatrix;
	zMATRIX4 camMatrixInv;

	zBOOL  tremorToggle;
	zREAL  tremorScale;
	zVEC3  tremorAmplitude;
	zVEC3  tremorOrigin;
	zREAL  tremorVelo;

	// Transformation matrices
	zMATRIX4  trafoView;
	zMATRIX4  trafoViewInv;
	zMATRIX4  trafoWorld;


	template <class T, int SIZE>
	class zCMatrixStack {
		int     pos;
		T       stack[SIZE];
	};

	zCMatrixStack<zMATRIX4,8> trafoViewStack;
	zCMatrixStack<zMATRIX4,8> trafoWorldStack;
	zCMatrixStack<zMATRIX4,8> trafoWorldViewStack;

	zMATRIX4 trafoProjection;

	enum { zTCAM_POLY_NUM_VERT = 4 };

	struct zTCamVertSimple {
		zREAL       x,y,z;
		zVEC2       texuv;
		zCOLOR      color;
	};

	zTCamVertSimple polyCamVerts[zTCAM_POLY_NUM_VERT];

	zCPolygon*   poly;
	zCMesh*      polyMesh;
	zCMaterial*  polyMaterial;

	// Screen-Effects
	zBOOL   screenFadeEnabled;
	zCOLOR  screenFadeColor;
	zSTRING screenFadeTexture;
	zREAL   screenFadeTextureAniFPS;

	zTRnd_AlphaBlendFunc screenFadeTextureBlendFunc;
	zBOOL cinemaScopeEnabled;
	zCOLOR cinemaScopeColor;

	//ungenutzt:
	enum zPROJECTION {
		PERSPECTIVE,
		ORTHOGONAL
	} projection;

	enum zTCam_DrawMode {
		zCAM_DRAW_NORMAL,
		zCAM_DRAW_NOTHING,
		zCAM_DRAW_WIRE,
		zCAM_DRAW_FLAT,
		zCAM_DRAW_TEX
	};
	zTCam_DrawMode drawMode;

	enum zTShadeMode {
		zSHADE_NORMAL,
		zSHADE_NOTHING,
		zSHADE_CONSTANT,
		zSHADE_GOURAUD,
		zSHADE_LIGHTMAP
	};
	zTShadeMode shadeMode;

	zBOOL drawWire;

	zVALUE    farClipZ;
	zVALUE    nearClipZ;
	zVALUE    viewDistanceX;
	zVALUE    viewDistanceY;
	zVALUE    viewDistanceXInv;
	zVALUE    viewDistanceYInv;
	zBOOL     vobFarClipZ;
	zREAL     fovH;
	zREAL     fovV;
	zCVob*    connectedVob;

	zVALUE    topBottomSin;
	zVALUE    topBottomCos;
	zVALUE    leftRightSin;
	zVALUE    leftRightCos;
};
