//
// Input.cpp
//

// Engine includes.
#include "Color.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "Input.h"

Input::Input(std::string name, df::TextBox *p_text_box) {
  setType("Input");
  setText("");
  setBorder(true);
  setColor(df::WHITE);
  setLocation(df::BOTTOM_CENTER);
  setLimit(40); // In characters
  m_p_text_box = p_text_box;
  m_name = name + ": ";
  setViewString(m_name);
}

// On callback, add message to TextBox.
void Input::callback() {

  LM.writeLog("Input::callback(): Input: %s", getText().c_str());
  if (getText() == "quit" || getText() == "exit")
    GM.setGameOver();

  // Add name to box.
  m_p_text_box -> addText(m_name);

  // Get text and add to box.
  m_p_text_box -> addText(getText());

  // Add newline to box.
  m_p_text_box -> addText("\n");
  
  // Clear for next entry.
  this -> clearText();
}