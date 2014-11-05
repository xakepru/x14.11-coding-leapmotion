#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "Cinder-LeapMotion.h"

class SkeletalApp : public ci::app::AppBasic
{
public:
	void						draw();
	void						prepareSettings( ci::app::AppBasic::Settings* settings );
	void						setup();
private:
	LeapMotion::DeviceRef		mDevice;
	Leap::Frame					mFrame;
	void 						onFrame( Leap::Frame frame );
	ci::CameraPersp				mCamera;
};

#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace LeapMotion;
using namespace std;

void SkeletalApp::draw()
{
	//gl::setViewport(getWindowBounds());
	gl::clear(Colorf::black());
	gl::setMatrices(mCamera);

	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::color(ColorAf::white());

	const Leap::HandList& hands = mFrame.hands();
	for ( Leap::HandList::const_iterator handIter = hands.begin(); handIter != hands.end(); ++handIter ) {
		const Leap::Hand& hand	= *handIter;
		const Leap::Arm& arm	= hand.arm();
		
		Vec3f elbow	= LeapMotion::toVec3f( arm.elbowPosition() );
		Vec3f wrist	= LeapMotion::toVec3f( arm.wristPosition() );

		gl::drawLine( elbow, wrist );

		vector<Vec3f> knuckles;
		const Leap::FingerList fingers = hand.fingers();
		for ( Leap::FingerList::const_iterator fingerIter = fingers.begin(); fingerIter != fingers.end(); ++fingerIter ) {
			const Leap::Finger& finger = *fingerIter;

			for ( int32_t i = 0; i < 4; ++i ) {
				const Leap::Bone& bone = finger.bone( (Leap::Bone::Type)i );
				Vec3f center	= LeapMotion::toVec3f( bone.center() );
				Vec3f direction	= LeapMotion::toVec3f( bone.direction() );
				Vec3f start		= center - direction * bone.length() * 0.5f;
				Vec3f end		= center + direction * bone.length() * 0.5f;
				
				if ( i == 0 ) {
					knuckles.push_back( start );
					gl::drawLine( wrist, start );
				} else {
					gl::drawLine( start, end );
				}
			}
		}

		if ( knuckles.size() > 1 ) {
			for ( size_t i = 1; i < knuckles.size(); ++i ) {
				const Vec3f& v0 = knuckles.at( i - 1 );
				const Vec3f& v1 = knuckles.at( i );
                gl::drawLine( v0,		v1 );
			}
			gl::drawLine( elbow, knuckles.at( 0 ) );
		}

	}
}

void SkeletalApp::onFrame( Leap::Frame frame )
{
	mFrame = frame;
}

void SkeletalApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
}

void SkeletalApp::setup()
{
	gl::enable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST ); 
	gl::enable( GL_POLYGON_SMOOTH );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 5000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 300.0f, 300.0f ), Vec3f( 0.0f, 250.0f, 0.0f ) );
	
	mDevice = Device::create();
	mDevice->connectEventHandler( [ & ]( Leap::Frame frame )
	{
		mFrame = frame;
	} );
}

CINDER_APP_BASIC( SkeletalApp, RendererGl )