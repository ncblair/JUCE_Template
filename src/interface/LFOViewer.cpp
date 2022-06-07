#include "LFOViewer.h"
#include "ValueHandle.h"
#include "../matrix/Matrix.h"
#include "../modulators/LFOModulator.h"
// #include "../modulators/NoteState.h"

//============================================================================================

LFOViewer::LFOViewer(Matrix* m, int modulator_id) {
    matrix = m;
    mod_id = modulator_id;
    lfo = matrix->getModulator(mod_id);
    lfo_tree = matrix->getLFOTree();

    recreate_handles();

    lfo_tree.addListener(this);
}

LFOViewer::~LFOViewer() {

}

void LFOViewer::paint (juce::Graphics& g) {

    auto r = proportionOfWidth(HANDLE_RADIUS);
    auto w = float(getWidth());
    auto h = float(getHeight());
    g.setColour (juce::Colours::white);
    g.fillRect(getLocalBounds());

    auto sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);

    auto num_points = sub_tree.getNumChildren();

    auto height = h - float(h)*float(sub_tree.getProperty(Matrix::LFO_HEIGHT_ID));
    
    handles[0]->setCentrePosition(0, height);
    handles[0]->constrain(0, 0, 0, h);
    auto x = 0.0;
    auto y = 1.0;
    auto prev_x = 0.0;
    auto prev_y = 1.0;
    if (num_points > 0) {
        auto first_point = sub_tree.getChild(0);
        x = float(first_point.getProperty(Matrix::LFO_POINT_X_ID));
        y = 1.0f - float(first_point.getProperty(Matrix::LFO_POINT_Y_ID));
        handles[1]->setCentrePosition(x*w / 2.0, (height + y*h) / 2.0);
        handles[1]->constrain(x*w / 2.0, 0, 0, h);
        handles[2]->setCentrePosition(x*w, y*h);
    }
    for (int i = 1; i < num_points; ++i) {
        auto point = sub_tree.getChild(i);
        auto next_x = float(point.getProperty(Matrix::LFO_POINT_X_ID));
        auto next_y = 1.0f - float(point.getProperty(Matrix::LFO_POINT_Y_ID));
        // TODO (we need to actually do a calculation to get the curved point value)
        handles[2*i]->constrain(prev_x*w, 0,(next_x - prev_x)*w, h);
        handles[1 + 2*i]->setCentrePosition((x + next_x)*w / 2.0, (y + next_y)*h/2.0);
        handles[1 + 2*i]->constrain((x + next_x)*w / 2.0, 0, 0, h);
        prev_x = x;
        prev_y = y;
        x = next_x;
        y = next_y;
        handles[2 + 2*i]->setCentrePosition(x*w, y*h);
    }
    if (num_points > 0) {
        handles[2*num_points]->constrain(prev_x*w, 0, (1.0 - prev_x)*w, h);
    }
    handles[1 + 2 * num_points]->setCentrePosition((x + 1.0)*w/2.0, (height + y*h)/2.0);
    handles[1 + 2 * num_points]->constrain((x + 1.0)*w/2.0, 0, 0, h);
    handles[2 + 2 * num_points]->setCentrePosition(w, height);
    handles[2 + 2 * num_points]->constrain(w, 0, 0, h);


    // draw the waveform
    juce::Path p;
    g.setColour (juce::Colours::black);
    p.startNewSubPath(0, h);

    int h_id = 0;
    for (int i = 0; i <= PRECISION; i++) {
        auto phase = float(i) / PRECISION;
        auto x_pos = w * phase;
        while (h_id < handles.size() && 
                handles[h_id]->isVisible() && 
                x_pos >= handles[h_id]->getX() + r) {
            p.lineTo(handles[h_id]->getX() + r, handles[h_id]->getY() + r);
            h_id++;
        }
        if (h_id >= handles.size()) {
            break;
        }
        else {
            //TODO: Draw in curves
        }
    }

    g.strokePath(p, juce::PathStrokeType(2));

    // for (int i = 0; i < handle_ids.size() - 1; ++i) {
    //     auto curve_handle = dynamic_cast<VerticalCurveHandle*>(handles[handle_ids[i]].get());
    //     if (curve_handle != nullptr && 
    //         (curve_handle->getX() == handles[handle_ids[i + 1]]->getX() ||
    //         curve_handle->getY() == handles[handle_ids[i + 1]]->getY())) {
    //             curve_handle->setVisible(false);
    //     }
    //     else if (handles[handle_ids[i]]->getX() > w + m) {
    //         handles[handle_ids[i]]->setVisible(false);
    //     }
    //     else {
    //         handles[handle_ids[i]]->setVisible(true);
    //     }
    // }

    // // constraints on mouse drag position
    // handles[LFO_PARAM::ATK_CURVE]->constrain(atk_curve_x, h + m, 0, -h);
    // handles[LFO_PARAM::ATK]->constrain(m, m, max_attack_x, 0);
    // handles[LFO_PARAM::DEC_CURVE]->constrain(dec_curve_x, m, 0, dec_y - m);
    // handles[LFO_PARAM::DEC]->constrain(atk_x, m, max_decay_x, h);
    // handles[LFO_PARAM::REL_CURVE]->constrain(rel_curve_x, dec_y, 0, h + m - dec_y);
    // handles[LFO_PARAM::REL]->constrain(dec_x, h + m, max_release_x, 0);
    // handles[LFO_PARAM::ATK]->set_total_length_ms(total_length_ms);
    // handles[LFO_PARAM::DEC]->set_total_length_ms(total_length_ms);
    // handles[LFO_PARAM::REL]->set_total_length_ms(total_length_ms);
}

void LFOViewer::resized() {
    auto diameter = 2.0f * proportionOfWidth(HANDLE_RADIUS);
    for (int i = 0; i < handles.size(); ++i) {
        handles[i]->setSize(diameter, diameter);
    }
}

void LFOViewer::recreate_handles() {
    //TODO maybe we don't need to do ALL this work
    auto undo_manager = matrix->getUndoManager();
    auto sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);
    handles.clear(); // should automatically remove the current handles from the scene
    handles.push_back(std::make_unique<VerticalValueHandle>(sub_tree, undo_manager, Matrix::LFO_HEIGHT_ID));
    handles.push_back(std::make_unique<VerticalCurveValueHandle>(sub_tree, undo_manager, Matrix::LFO_CURVE_1_ID));
    for (int i = 0; i < sub_tree.getNumChildren(); ++i) {
        auto child_tree = sub_tree.getChild(i);
        handles.push_back(std::make_unique<FreeValueHandle>(child_tree, undo_manager, Matrix::LFO_POINT_X_ID, Matrix::LFO_POINT_Y_ID));
        handles.push_back(std::make_unique<VerticalCurveValueHandle>(child_tree, undo_manager, Matrix::LFO_POINT_CURVE_ID));
    }
    handles.push_back(std::make_unique<VerticalValueHandle>(sub_tree, undo_manager, Matrix::LFO_HEIGHT_ID));
    for (int i = 0; i < handles.size(); ++i) {
        addAndMakeVisible(*(handles[i]));
    }
    resized();
}

void LFOViewer::mouseDoubleClick (const MouseEvent& e) {
    // get index first
    auto position = e.getMouseDownPosition().toFloat();
    auto size = juce::Point<float>(getWidth(), getHeight());
    auto normed_position = position / size;

    auto lfo_point = juce::ValueTree(Matrix::LFO_POINT_ID);
    auto x = normed_position.getX();
    auto y = 1.0f - normed_position.getY();
    lfo_point.setProperty(Matrix::LFO_POINT_X_ID, x, nullptr);
    lfo_point.setProperty(Matrix::LFO_POINT_Y_ID, y, nullptr);
    lfo_point.setProperty(Matrix::LFO_POINT_CURVE_ID, 1.0f, nullptr);
    auto index = 0;
    auto sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);
    while (index < sub_tree.getNumChildren() && x > float(sub_tree.getChild(index).getProperty(Matrix::LFO_POINT_X_ID))) {
        index++;
    }
    sub_tree.addChild(lfo_point, index, matrix->getUndoManager());


    // matrix->add_lfo_point(mod_id, normed_position);
}

void LFOViewer::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) {
    if (treeWhosePropertyHasChanged.hasType(MODULATOR_IDS[mod_id]) || 
        treeWhosePropertyHasChanged.getParent().hasType(MODULATOR_IDS[mod_id])) 
    {
        repaint();
    }
    
}
void LFOViewer::valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) {
    // add a handle
    if (parentTree.hasType(MODULATOR_IDS[mod_id])) {
        recreate_handles();
        repaint();
    }
    else if (parentTree.hasType(Matrix::LFO_TREE_ID)) {
        // new tree loaded entirely, via loading a preset or etc
        recreate_handles();
        repaint();
    }
}
void LFOViewer::valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) {
    // remove a handle
    if (parentTree.hasType(MODULATOR_IDS[mod_id])) {
        recreate_handles();
        repaint();
    }
}