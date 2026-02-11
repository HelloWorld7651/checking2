//
  // Sentry.h - Poll NetworkManager for incoming messages.
  //

  // Engine includes.
  #include "Object.h"
  #include "TextEntry.h"

  namespace df {

  class Sentry : public Object {

   private:
    void handleStep();

   public:
    Sentry();
    int eventHandler(const Event *p_e) override;
  };

  } // end of namespace df
