#include "TextureEditor.h"

namespace ofx{
    namespace piMapper{
TextureEditor::TextureEditor()
{
    clear();
    enable();
}

TextureEditor::~TextureEditor()
{
    clear();
    disable();
}

void TextureEditor::registerAppEvents()
{
    ofAddListener(ofEvents().update, this, &TextureEditor::update);
}

void TextureEditor::unregisterAppEvents()
{
    ofRemoveListener(ofEvents().update, this, &TextureEditor::update);
}

void TextureEditor::registerKeyEvents()
{
    ofAddListener(ofEvents().keyPressed, this, &TextureEditor::keyPressed);
    ofAddListener(ofEvents().keyReleased, this, &TextureEditor::keyReleased);
}

void TextureEditor::unregisterKeyEvents()
{
    ofRemoveListener(ofEvents().keyPressed, this, &TextureEditor::keyPressed);
    ofRemoveListener(ofEvents().keyReleased, this, &TextureEditor::keyReleased);
}

void TextureEditor::enable()
{
    registerAppEvents();
    registerKeyEvents();
    bShiftKeyDown = false;
}

void TextureEditor::disable()
{
    unregisterAppEvents();
    unregisterKeyEvents();
}

void TextureEditor::update(ofEventArgs &args)
{
    if ( surface == NULL ) return;
    
    // update surface if one of the joints is being dragged
    ofVec2f textureSize = ofVec2f( surface->getTexture()->getWidth(), surface->getTexture()->getHeight() );
    for ( int i=0; i<joints.size(); i++ ) {
        if ( joints[i]->isDragged() || joints[i]->isSelected() ) {
            // update vertex to new location
            surface->setTexCoord(i, joints[i]->position / textureSize);
            break;
        }
    }
}

void TextureEditor::keyPressed(ofKeyEventArgs &args)
{
    int key = args.key;
    float moveStep;
    
    if (bShiftKeyDown) moveStep = 10.0f;
    else moveStep = 0.5f;
    
    switch (key) {
        case OF_KEY_LEFT: moveSelection(ofVec2f(-moveStep,0.0f)); break;
        case OF_KEY_RIGHT: moveSelection(ofVec2f(moveStep,0.0f)); break;
        case OF_KEY_UP: moveSelection(ofVec2f(0.0f,-moveStep)); break;
        case OF_KEY_DOWN: moveSelection(ofVec2f(0.0f,moveStep)); break;
        case OF_KEY_SHIFT: bShiftKeyDown = true; break;
    }
}

void TextureEditor::keyReleased(ofKeyEventArgs &args)
{
    int key = args.key;
    switch (key) {
        case OF_KEY_SHIFT: bShiftKeyDown = false; break;
    }
}


void TextureEditor::draw()
{
    if (surface == NULL) return;
    
    drawJoints();
}

void TextureEditor::drawJoints()
{
    for ( int i=0; i<joints.size(); i++ ) {
        joints[i]->draw();
    }
}

void TextureEditor::setSurface(BaseSurface* newSurface)
{
    surface = newSurface;
    createJoints();
}

void TextureEditor::clear()
{
    surface = NULL;
    clearJoints();
}

void TextureEditor::createJoints()
{
    if ( surface == NULL ) return;
    clearJoints();
    vector<ofVec2f>& texCoords = surface->getTexCoords();
    ofVec2f textureSize = ofVec2f(surface->getTexture()->getWidth(), surface->getTexture()->getHeight());
    
    for ( int i=0; i<texCoords.size(); i++ ) {
        joints.push_back(new CircleJoint());
        joints.back()->position = texCoords[i] * textureSize;
    }
}

void TextureEditor::clearJoints()
{
    while ( joints.size() ) {
        delete joints.back();
        joints.pop_back();
    }
}

void TextureEditor::unselectAllJoints()
{
    for ( int i=0; i<joints.size(); i++ ) {
        joints[i]->unselect();
    }
}

void TextureEditor::moveTexCoords(ofVec2f by)
{
    if ( surface == NULL ) return;
    vector<ofVec2f>& texCoords = surface->getTexCoords();
    ofVec2f textureSize = ofVec2f( surface->getTexture()->getWidth(), surface->getTexture()->getHeight() );
    for (int i=0; i<texCoords.size(); i++) {
        joints[i]->position += by;
        texCoords[i] = joints[i]->position / textureSize;
    }
}

void TextureEditor::stopDragJoints()
{
    for (int i=0; i<joints.size(); i++){
        joints[i]->stopDrag();
    }
}

void TextureEditor::moveSelection(ofVec2f by)
{
    // check if joints selected
    bool bJointSelected = false;
    BaseJoint* selectedJoint;
    for ( int i=0; i<joints.size(); i++ ) {
        if (joints[i]->isSelected()) {
            bJointSelected = true;
            selectedJoint = joints[i];
            break;
        }
    }
    
    if ( bJointSelected ) {
        selectedJoint->position += by;
    } else {
        moveTexCoords(by);
    }
}

CircleJoint* TextureEditor::hitTestJoints(ofVec2f pos)
{
    for ( int i=0; i<joints.size(); i++ ) {
        if ( joints[i]->hitTest(pos) ){
            return joints[i];
        }
    }
    return NULL;
}

    }}