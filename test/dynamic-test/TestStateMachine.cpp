
// Assume existing a dancing robot:
// It has 3 states:          { closed,  opened,  dancing }
// It can receive 4 events : { close,   open,    play,   stop }
// It can do 4 actions :     { sayReady,  sayClosed,  doDance,  sayStoped}

// The following table describes how the robot acting in detail!

/*******************************************************************/
/*  Current State |   Event    |    Next State     |   Action      */
/*----------------|------------|-------------------|---------------*/
/*   closed       |   open     |     opened        |   sayReady    */
/*----------------|------------|-------------------|---------------*/
/*   opened       |   close    |     closed        |   sayClosed   */
/*----------------|------------|-------------------|---------------*/
/*   opened       |   play     |     dancing       |   doDance     */
/*----------------|------------|-------------------|---------------*/
/*   dancing      |   stop     |     opened        |   sayStoped   */
/*----------------|------------|-------------------|---------------*/
/*   dancing      |   close    |     closed        |   sayClosed   */
/*******************************************************************/

#include "gtest.h"

#include <string>

#include "loki/StateMachine.h"

// Events
struct Close {};
struct Open {};
struct Play {
  std::string name;
};
struct Stop {};

struct DanceRobot : LOKI_NS::StateMachine<DanceRobot> {
 public:
  enum States {
      closed, opened, dancing, initial = closed
  };
 private:
  friend struct StateMachine<DanceRobot>;

  // actions
  void sayReady(const Open&) {
    std::cout << "Robot is ready for play!" << std::endl;
  }

  void sayClosed(const Close&) {
    std::cout << "Robot is closed!" << std::endl;
  }

  void sayStoped(const Stop&) {
    std::cout << "Robot stops playing!" << std::endl;
  }

  void doDance(const Play& playInfo) {
    std::cout << "Robot is dancing (" << playInfo.name << ") now!" << std::endl;
  }

    // table
  using R = DanceRobot;

  using TransitionTable = __type_list(
  //  +----------+----------+----------+----------------+
  //  |  current |   event  |  target  |  action        |
  //  +----------+----------+----------+----------------+
  Row <  closed  ,   Open   ,  opened  ,  &R::sayReady  >,
  //  +----------+----------+----------+----------------+
  Row <  opened  ,   Close  ,  closed  ,  &R::sayClosed >,
  Row <  opened  ,   Play   ,  dancing ,  &R::doDance   >,
  //  +----------+----------+----------+----------------+
  Row <  dancing ,   Stop   ,  opened  ,  &R::sayStoped >,
  Row <  dancing ,   Close  ,  closed  ,  &R::sayClosed >
  //  +----------+----------+----------+----------------+
  );
};

TEST(DanceRobot, Basic) {
  DanceRobot robot;

  ASSERT_EQ(DanceRobot::States::opened, robot.processEvent(Open()));
  ASSERT_EQ(DanceRobot::States::closed, robot.processEvent(Close()));
  ASSERT_EQ(DanceRobot::States::opened, robot.processEvent(Open()));
  ASSERT_EQ(DanceRobot::States::dancing,
            robot.processEvent(Play{.name = "hip-hop"}));
  ASSERT_EQ(DanceRobot::States::opened,
            robot.processEvent(Stop()));
  ASSERT_EQ(DanceRobot::States::closed,
            robot.processEvent(Close()));
}
