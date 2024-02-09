#include <Geode/Geode.hpp>
#include <Geode/utils/NodeIDs.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;
using namespace geode::node_ids;

float musicVolume = GameManager::get()->m_bgVolume;
float sfxVolume = GameManager::get()->m_sfxVolume;

void updateVolume(CCObject* slTh, bool music){
	SliderThumb* actualVolume = static_cast<SliderThumb*>(slTh);
	if(music == true)
		musicVolume = actualVolume->getValue();
	else
		sfxVolume = actualVolume->getValue();
}

class $modify(EditorPauseLayerWithVolume,EditorPauseLayer) {
	bool init(LevelEditorLayer* p0){
		if(!EditorPauseLayer::init(p0))
			return false;

		// Move the guidelines-menu buttons to the top-menu.
		CCNode* guidelinesButtons = this->getChildByID("guidelines-menu");
		CCNode* topPosition = this->getChildByID("top-menu");
		for(int i=0;i<=guidelinesButtons->getChildrenCount();i++)
			switchChildToMenu(guidelinesButtons,0,topPosition);
		topPosition->updateLayout();

		// Modifications to the EditorPauseLayer layout.
		auto winSize = CCDirector::get()->getWinSize();
		guidelinesButtons->setContentSize({winSize.width,guidelinesButtons->getContentSize().height});
		CCNode* menuTemp = this->getChildByID("options-menu");
		menuTemp->setLayout(
			RowLayout::create()
				->setAxisAlignment(AxisAlignment::Start)
				->setCrossAxisAlignment(AxisAlignment::End)
				->setGrowCrossAxis(true)
				->setCrossAxisOverflow(false)
				->setGap(3.5)
 		);
		menuTemp=this->getChildByID("small-actions-menu");
		menuTemp->setLayout(
			RowLayout::create()
				->setAxis(Axis::Column)
				->setAxisAlignment(AxisAlignment::End)
				->setCrossAxisOverflow(false)
 		);
		menuTemp->setPositionY(winSize.height-menuTemp->getContentSize().height/2-5);
		menuTemp=this->getChildByID("settings-menu");
		menuTemp->setPositionY(winSize.height-menuTemp->getContentSize().height/2-5);
		menuTemp=this->getChildByID("actions-menu");
		menuTemp->setPositionY(winSize.height-menuTemp->getContentSize().height/2+this->getChildByID("settings-menu")->getContentSize().height/2-5); // sorry

		// Make the sliders.
		Slider* musicSlider = Slider::create(guidelinesButtons, menu_selector(EditorPauseLayerWithVolume::onMusicChanged));
		Slider* sfxSlider = Slider::create(guidelinesButtons, menu_selector(EditorPauseLayerWithVolume::onSfxChanged));
		// Change sizes and add value of the sliders.
		getChildOfType<SliderTouchLogic>(musicSlider,0)->m_length=160;
		getChildOfType<CCSprite>(musicSlider,0)->setScale(0.8);
		getChildOfType<SliderThumb>(getChildOfType<SliderTouchLogic>(musicSlider,0),0)->setScale(0.8);
		musicSlider->setValue(musicVolume);
		getChildOfType<SliderTouchLogic>(sfxSlider,0)->m_length=160;
		getChildOfType<CCSprite>(sfxSlider,0)->setScale(0.8);
		getChildOfType<SliderThumb>(getChildOfType<SliderTouchLogic>(sfxSlider,0),0)->setScale(0.8);
		sfxSlider->setValue(sfxVolume);
		// Place the sliders manually (Thanks Robtop or Coco2d that the sliders are huge for some reason).
		float sliderX = getChildOfType<CCSprite>(musicSlider,0)->getContentSize().width * 0.8;
		float sliderY = getChildOfType<SliderThumb>(getChildOfType<SliderTouchLogic>(sfxSlider,0),0)->getContentSize().height * 0.8 - 7.5;
		float spacing = (winSize.width - (sliderX * 2 + sliderY))/2;
		musicSlider->setPosition(spacing + sliderX / 2, guidelinesButtons->getPositionY()/2);
		sfxSlider->setPosition(spacing + sliderX * 1.5 + sliderY, guidelinesButtons->getPositionY()/2);
		// Add the sliders to guidelines-menu.
		guidelinesButtons->addChild(musicSlider);
		guidelinesButtons->addChild(sfxSlider);
		
		//Make and add Labels "Music" and "SFX".
		CCLabelBMFont* lblMusic = CCLabelBMFont::create("Music","bigFont.fnt");
		CCLabelBMFont* lblSfx = CCLabelBMFont::create("SFX","bigFont.fnt");
		lblMusic->setScale(0.5);
		lblSfx->setScale(0.5);
		lblMusic->setAnchorPoint({0.5,0});
		lblSfx->setAnchorPoint({0.5,0});
		lblMusic->setPosition(spacing + sliderX / 2, guidelinesButtons->getPositionY()/2 + sliderY/2);
		lblSfx->setPosition(spacing + sliderX * 1.5 + sliderY, guidelinesButtons->getPositionY()/2 + sliderY/2);
		guidelinesButtons->addChild(lblMusic);
		guidelinesButtons->addChild(lblSfx);
		return true;
	}

	// Callbacks
	void onMusicChanged(CCObject* sender){
		OptionsLayer* optionsTemp = OptionsLayer::create();
		optionsTemp->musicSliderChanged(sender);
		FMODAudioEngine::sharedEngine()->stopAllMusic(); // Prevent the menu song starts. Honestly im surprised it callback works.
	}
	void onSfxChanged(CCObject* sender){
		OptionsLayer* optionsTemp = OptionsLayer::create();
		optionsTemp->sfxSliderChanged(sender);
	}
};

// Update PauseLayer and OptionsLayers so that global variables are updated.
class $modify(PauseLayer){
	TodoReturn musicSliderChanged(cocos2d::CCObject* p0){
		updateVolume(p0,true);
		PauseLayer::musicSliderChanged(p0);
	}
	TodoReturn sfxSliderChanged(cocos2d::CCObject* p0){
		updateVolume(p0,false);
		PauseLayer::sfxSliderChanged(p0);
	}
};
class $modify(OptionsLayer){
	TodoReturn musicSliderChanged(cocos2d::CCObject* p0){
		updateVolume(p0,true);
		OptionsLayer::musicSliderChanged(p0);
	}
	TodoReturn sfxSliderChanged(cocos2d::CCObject* p0){
		updateVolume(p0,false);
		OptionsLayer::sfxSliderChanged(p0);
	}
};