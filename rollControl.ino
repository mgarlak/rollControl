void setup() {
  // put your setup code here, to run once:
  int flightMode=0
}



void loop() {
  //any code that needs to run every loop regardless of flightMode.
  case(flightMode){
    case 0 : 
      //On the ground
      break;
    case 1:
      //boost phase
      break;
    case 2:
      //Coast phase, where we control roll
      break;
    case 3:
      //Decent phase, initial
      break;
    case 4:
      //Decent phase, after main chute deply
      break;
    case 5:
      //on ground
    
  }
}
