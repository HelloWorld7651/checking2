//
// Input.h - Uses TextEntry to get chat input.
//

// System includes.
#include <string>

// Engine includes.
#include "TextBox.h"
#include "TextEntry.h"

class Input : public df::TextEntry {

 private:
  df::TextBox *m_p_text_box;  // Text box for chat messages.
  std::string m_name;         // Name of chatter.
  
 public:
  Input(std::string name, df::TextBox *p_text_box);
  void callback() override;
};
