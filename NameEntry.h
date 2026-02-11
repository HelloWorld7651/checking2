//
// NameEntry.h - Uses TextEntry to get name of chatter.
//

// Engine includes.
#include "TextEntry.h"

class NameEntry : public df::TextEntry {

 public:
  NameEntry();
  void callback() override;
};