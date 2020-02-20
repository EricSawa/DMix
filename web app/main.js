let terminalContainer = document.getElementById('terminal');
let sendForm = document.getElementById('send-form');
let inputField = document.getElementById('input');

/******************************************
Definitions
******************************************/
let numberTransitions = 20;
let numberPresets = 10;
let numberFootswitch = 4;
/******************************************
Storage
******************************************/
let _dictPreset = [];
let _dictFootswitch = [];
let _activePreset = 0;
/******************************************
HTML Elemente
******************************************/
// Navigation Bar
var navigationHtml = [
  '<div class="loaderInvisable"></div>',
  '<div class="navigation w-row">',
    '<div class ="nav_col1 w-col w-col-2 w-col-small-2 w-col-tiny-2">',
      '<div class="nav_col1text">DMiX</div>',
    '</div>',
    '<div class="nav_col2 w-col w-col-6 w-col-small-6 w-col-tiny-6"></div>',
    '<div class="nav_col3usb w-col w-col-1 w-col-small-1 w-col-tiny-1"><img src="images/usb.png" alt="" class="nav_usbImageInvisable" ></div>',
    '<div class="nav_col4bt w-col w-col-1 w-col-small-1 w-col-tiny-1"><img src="images/bt.png" alt="" class="nav_btImageInvisable" onclick="bt_onDisconnection()"></div>',
    '<div class="nav_col5menu w-col w-col-2 w-col-small-2 w-col-tiny-2"></div>',
  '</div>'
].join("\n");
// Login Screen
var loginHtml = [
  '<div class="loginInvisable">',
    '<h1 = class="heading-1">- SELECT YOUR INTERFACE - </h1>',
    '<div class="interfaceVisable">',
      '<div class="interfaceButton" onclick="bt_connect()">',
        '<div class="div_interfaceButton"><img src="images/bt.png" alt="" class="image_interfaceButton">',
          '<div class="text_interfaceButton">BLUETOOTH</div>',
        '</div>',
      '</div>',
      '<div class="interfaceButton">',
        '<div class="div_interfaceButton"><img src="images/usb.png" alt="" class="image_interfaceButton">',
          '<div class="text_interfaceButton">USB</div>',
        '</div>',
      '</div>',
    '</div>',
  '</div>'
].join("\n");

// Preset Screen
var presetHtml = [
  '<div class="presetInvisable">',
    '<h1 = class="heading-1">- PRESETS -</h1>',
    '<div class="presetNumber">',
      '<div class="div_preset_padding"></div>',
      //Preset Number
      '<div class="div_prnmbr">',
        '<div class="div_prnmbr_header">',
          '<div class="text_preset">NUMBER</div>',
        '</div>',
        '<div class="div_prnmbr_selection">',
          '<select class="select_prnmbr_dropdown">',
          '</select>',
        '</div>',
      '</div>',
      //Preset Name
      '<div class="div_prname">',
        '<div class="div_prname_header">',
          '<div class="text_preset">NAME</div>',
        '</div>',
        '<div class="div_prname_value">',
          '<form id="form_prname" onSubmit="return false;">',
            '<input class="input_prname" type="text" placeholder="Example Preset">',
          '</form>',
        '</div>',
      '</div>',
    '</div>',
    //Preset Transitions
    '<h1 = class="heading-1">- TRANSITIONS -</h1>',
    '<div class="presetTransitions">',
  '</div>',
].join("\n");

var presetTransitionHtml = [
    '<div class="div_transition">',
      //Number
      '<div class="div_transition_nmbr">',
        '<div class="div_transition_header"></div>',
        '<div class="text_preset_nr">1</div>',
      '</div>',
      //Channel
      '<div class="div_transition_ch">',
        '<div class="div_transition_header">',
          '<div class="text_preset">CH</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<form id="form_transition" onSubmit="return false;">',
            '<input class="input_transition_ch" type="number" min="0" max="255" step="1" placeholder="0"  key="channel">',
          '</form>',
        '</div>',
      '</div>',
      //Start
      '<div class="div_transition_start">',
        '<div class="div_transition_header">',
          '<div class="text_preset">START</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<form id="form_transition" onSubmit="return false;">',
            '<input class="input_transition_start"  type="number" min="0" max="255" step="1" placeholder="0"  key="startValue">',
          '</form>',
        '</div>',
      '</div>',
      //End
      '<div class="div_transition_end">',
        '<div class="div_transition_header">',
          '<div class="text_preset">END</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<form id="form_transition" onSubmit="return false;">',
            '<input class="input_transition_end"  type="number" min="0" max="255" step="1" placeholder="0"  key="endValue">',
          '</form>',
        '</div>',
      '</div>',
      //Time
      '<div class="div_transition_time">',
        '<div class="div_transition_header">',
          '<div class="text_preset">TIME</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<form id="form_transition" onSubmit="return false;">',
            '<input class="input_transition_time"  type="number" min="0" max="32767" step="1" placeholder="0" key="transitionTime">',
          '</form>',
        '</div>',
      '</div>',
      //Delay
      '<div class="div_transition_delay">',
        '<div class="div_transition_header">',
          '<div class="text_preset">DELAY</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<form id="form_transition" onSubmit="return false;">',
            '<input class="input_transition_delay"  type="number" min="0" max="32767" step="1" placeholder="0"  key="delay">',
          '</form>',
        '</div>',
      '</div>',
      //Ramp
      '<div class="div_transition_ramp">',
        '<div class="div_transition_header">',
          '<div class="text_preset">RAMP</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<select class="select_transition_ramp" key="ramp">',
            '<option value=0>LIN</option>',
          '</select>',
        '</div>',
      '</div>',
      //Active
      '<div class="div_transition_active">',
        '<div class="div_transition_header">',
          '<div class="text_preset">ACTIVE</div>',
        '</div>',
        '<div class="div_transition_value">',
          '<input type="checkbox" class="checkbox_transition_active" name="zutat" key="active">',
        '</div>',
      '</div>',
].join("\n");

// Footswitch Screen
var footswitchHtml = [
  '<div class="footswitchInvisable">',
    '<div class="div_footswitch">',
    '</div>',
  '</div>',
].join("\n");
//
var footswitchParamsHtml = [
  '<div class="div_footswitch_param">',
  '<h1 = class="heading-1">- -</h1>',
    //Pressed
    '<div class="div_footswitch_cfg">',
      '<div class="div_footswitch_control">',
        '<div class="div_footswitch_empty"></div>',
        '<div class="div_footswitch_name">',
          '<div class="text_preset">PRESSED</div>',
        '</div>',
        '<div class="div_footswitch_form">',
          '<select class="select_footswitch_preset_pressed"  key="pressed">',
          '</select>',
        '</div>',
        '<button class="btn_footswitch_test" type="button">TEST</button>',
      '</div>',
    '</div>',
    //Released
    '<div class="div_footswitch_cfg">',
      '<div class="div_footswitch_control">',
        '<div class="div_footswitch_empty"></div>',
        '<div class="div_footswitch_name">',
          '<div class="text_preset">RELEASED</div>',
        '</div>',
        '<div class="div_footswitch_form">',
          '<select class="select_footswitch_preset_released"  key="released">',
          '</select>',
        '</div>',
        '<button class="btn_footswitch_test" type="button">TEST</button>',
      '</div>',
    '</div>',
    //Hold short
    '<div class="div_footswitch_cfg">',
      '<div class="div_footswitch_control">',
        '<div class="div_footswitch_empty"></div>',
        '<div class="div_footswitch_name">',
          '<div class="text_preset">HOLD_SHORT</div>',
        '</div>',
        '<div class="div_footswitch_form">',
          '<select class="select_footswitch_preset_hold_short"  key="hold_short">',
          '</select>',
        '</div>',
        '<button class="btn_footswitch_test" type="button">TEST</button>',
      '</div>',
    '</div>',
    //Hold long
    '<div class="div_footswitch_cfg">',
      '<div class="div_footswitch_control">',
        '<div class="div_footswitch_empty"></div>',
        '<div class="div_footswitch_name">',
          '<div class="text_preset">HOLD_LONG</div>',
        '</div>',
        '<div class="div_footswitch_form">',
          '<select class="select_footswitch_preset_hold_long"  key="hold_long">',
          '</select>',
        '</div>',
        '<button class="btn_footswitch_test" type="button">TEST</button>',
      '</div>',
    '</div>',
  '</div>',
].join("\n");
/******************************************
Storage Elemente
******************************************/
function createStorage(){
    //Generate preset array
    for(var j = 0; j < numberPresets; ++j){
      let dictTransition = [];
      for (var i = 0; i < numberTransitions; i++){
        dictTransition.push({
          "channel": 0,
          "startValue": 0,
          "endValue": 0,
          "transitionTime": 0,
          "delay": 0,
          "ramp": 0,
          "active" : false
        });
      }
      _dictPreset.push({
        "Name": "Preset " + j,
        "Params": dictTransition
      });
    }
    //Generate footswitch array
    for(var j = 0; j < numberFootswitch; ++j){
      _dictFootswitch.push({
        "pressed": 0,
        "released": 0,
        "hold_short": 0,
        "hold_long": 0,
      });
    }
}

function createPresetDropdown(){
  let presetNumber = document.querySelectorAll('.select_prnmbr_dropdown');
  let presetName = document.querySelectorAll('.input_prname');
  //Fill PRESET dropdown list
  for(let j = 0; j < numberPresets; ++j){
    // create new option element
    let opt = document.createElement('option');
    opt.value = j;
    presetNumber[0].add(opt);
  }
  //Fill Footswitch dropdown
  let pressedOptions = document.querySelectorAll('.select_footswitch_preset_pressed');
  let releasedOptions = document.querySelectorAll('.select_footswitch_preset_released');
  let holdShortOptions = document.querySelectorAll('.select_footswitch_preset_hold_short');
  let holdLongOptions = document.querySelectorAll('.select_footswitch_preset_hold_long');
  for(let i = 0; i < pressedOptions.length; ++i){
    for(let j = 0; j < numberPresets; ++j){
      // create new option element
      let opt1 = document.createElement('option');
      let opt2 = document.createElement('option');
      let opt3 = document.createElement('option');
      let opt4 = document.createElement('option');
      opt1.value = opt2.value = opt3.value = opt4.value = j;
      pressedOptions[i].add(opt1);
      releasedOptions[i].add(opt2);
      holdShortOptions[i].add(opt3);
      holdLongOptions[i].add(opt4);
    }
  }
  //Update dropdown list
  updatePresetDropdown();
  //Load last NAME
  presetName[0].value = _dictPreset[_activePreset].Name;
}

function updatePresetDropdown(){
  //Update PRESETS dropdown
  let presetNumberOptions = document.querySelectorAll('.select_prnmbr_dropdown');
  let options = presetNumberOptions[0].options;
  for(let i = 0; i < options.length; ++i){
    options[i].text = i + " - [" + _dictPreset[i].Name + "]";
  }
  //Update Footswitch dropdown
  let pressedOptions = document.querySelectorAll('.select_footswitch_preset_pressed');
  let releasedOptions = document.querySelectorAll('.select_footswitch_preset_released');
  let holdShortOptions = document.querySelectorAll('.select_footswitch_preset_hold_short');
  let holdLongOptions = document.querySelectorAll('.select_footswitch_preset_hold_long');
  for(let i = 0; i < pressedOptions.length; ++i){
    let newPressedOptions = pressedOptions[i].options;
    let newReleasedOptions = releasedOptions[i].options;
    let newHoldShortOptions = holdShortOptions[i].options;
    let newHoldLongOptions = holdLongOptions[i].options;
    for(let j = 0; j < newPressedOptions.length; ++j){
      newPressedOptions[j].text = j + " - [" + _dictPreset[j].Name + "]";
      newReleasedOptions[j].text = j + " - [" + _dictPreset[j].Name + "]";
      newHoldShortOptions[j].text = j + " - [" + _dictPreset[j].Name + "]";
      newHoldLongOptions[j].text = j + " - [" + _dictPreset[j].Name + "]";
    }
  }
}
/******************************************
Logic
******************************************/
function onTransitionChange(){
  let max = parseInt(this.getAttribute('max'));
  let min = parseInt(this.getAttribute('min'));
  let value = this.value;
  let key = this.getAttribute('key');
  let idx = this.getAttribute('idx');
  //Clamp values
  if(this.getAttribute('key') == "active"){
      value = this.checked;
      min = 0;
      max = 1;
  }
  if(value > max) {
    this.value = max;
  }else if(value < min){
      this.value = min;
  }
  //Load values into storage
  _dictPreset[_activePreset]["Params"][idx][key] = value;
  console.log(_dictPreset[_activePreset]["Params"][idx]);
}

function onPresetNumberChange(){
  //Change presetNumber
  _activePreset = this.value;
  updatePreset();
}

function updatePreset(){
  let presetName = document.querySelectorAll('.input_prname');
  let presetNumber = document.querySelectorAll('.select_prnmbr_dropdown');
  presetName[0].value = _dictPreset[_activePreset].Name;
  presetNumber[0].value = _activePreset;
  var transitionChannel = document.querySelectorAll('.input_transition_ch');
  var transitionStart = document.querySelectorAll('.input_transition_start');
  var transitionEnd = document.querySelectorAll('.input_transition_end');
  var transitionTime = document.querySelectorAll('.input_transition_time');
  var transitionDelay = document.querySelectorAll('.input_transition_delay');
  var transitionRamp = document.querySelectorAll('.select_transition_ramp');
  var transitionActive = document.querySelectorAll('.checkbox_transition_active');
  for (var i = 0; i < transitionChannel.length; i++){
    transitionChannel[i].value = _dictPreset[_activePreset]["Params"][i]["channel"];
    transitionStart[i].value = _dictPreset[_activePreset]["Params"][i]["startValue"];
    transitionEnd[i].value = _dictPreset[_activePreset]["Params"][i]["endValue"];
    transitionTime[i].value = _dictPreset[_activePreset]["Params"][i]["transitionTime"];
    transitionDelay[i].value = _dictPreset[_activePreset]["Params"][i]["delay"];
    transitionRamp[i].value = _dictPreset[_activePreset]["Params"][i]["ramp"];
    transitionActive[i].checked = _dictPreset[_activePreset]["Params"][i]["active"];
  }
}

function onPresetNameChange(){
  presetName = this.value;
  _dictPreset[_activePreset].Name = presetName;
  updatePresetDropdown();
}

function onFootswitchChange(){
  let key = this.getAttribute('key');
  let idx = this.getAttribute('idx');
  let value = this.value;
  //Load values into storage
  _dictFootswitch[idx][key] = value;
  console.log(_dictFootswitch[idx]);
}

 // TODO: onFootswitchChange
 // TODO: Tabs select
 // TODO: Define protocoll
 // TODO: Print protocoll on change

/******************************************
Generate Page
******************************************/
function insertNavigation(){
  document.body.insertAdjacentHTML( 'beforeend', navigationHtml );
}
function insertLogin(){
  document.body.insertAdjacentHTML( 'beforeend', loginHtml );
}
function insertPreset(){
  document.body.insertAdjacentHTML( 'beforeend', presetHtml );
  var x = document.getElementsByClassName("presetTransitions");
  for (var i = 0; i < numberTransitions; i++){
    x[0].insertAdjacentHTML('beforeend', presetTransitionHtml );
  }
  //Add functions to PRESETS
  let presetNumber = document.querySelectorAll('.select_prnmbr_dropdown');
  presetNumber[0].onchange = onPresetNumberChange;
  let presetName = document.querySelectorAll('.input_prname');
  presetName[0].onchange = onPresetNameChange;
  //Add functions to transitions
  var transitionNumber = document.querySelectorAll('.div_transition_nmbr > .text_preset_nr');
  var transitionChannel = document.querySelectorAll('.input_transition_ch');
  var transitionStart = document.querySelectorAll('.input_transition_start');
  var transitionEnd = document.querySelectorAll('.input_transition_end');
  var transitionTime = document.querySelectorAll('.input_transition_time');
  var transitionDelay = document.querySelectorAll('.input_transition_delay');
  var transitionRamp = document.querySelectorAll('.select_transition_ramp');
  var transitionActive = document.querySelectorAll('.checkbox_transition_active');
  for (var i = 0; i < transitionNumber.length; i++){
    //Count up the transisiton number
    transitionNumber[i].innerHTML = i;
    transitionChannel[i].setAttribute('idx', i);
    transitionChannel[i].onchange = onTransitionChange;
    transitionStart[i].setAttribute('idx', i);
    transitionStart[i].onchange = onTransitionChange;
    transitionEnd[i].setAttribute('idx', i);
    transitionEnd[i].onchange = onTransitionChange;
    transitionTime[i].setAttribute('idx', i);
    transitionTime[i].onchange = onTransitionChange;
    transitionDelay[i].setAttribute('idx', i);
    transitionDelay[i].onchange = onTransitionChange;
    transitionRamp[i].setAttribute('idx', i);
    transitionRamp[i].onchange = onTransitionChange;
    transitionActive[i].setAttribute('idx', i);
    transitionActive[i].onchange = onTransitionChange;
  }
}

function insertFootswitch(){
  document.body.insertAdjacentHTML( 'beforeend', footswitchHtml );
  var x = document.getElementsByClassName("div_footswitch");
  for (var i = 0; i < numberFootswitch; i++){
    x[0].insertAdjacentHTML('beforeend', footswitchParamsHtml );
  }
  var footswitchHeader = document.querySelectorAll('.div_footswitch_param > .heading-1');
  var footswitchPresetSelectPressed = document.querySelectorAll('.select_footswitch_preset_pressed');
  var footswitchPresetSelectReleased = document.querySelectorAll('.select_footswitch_preset_released');
  var footswitchPresetSelectHoldShort = document.querySelectorAll('.select_footswitch_preset_hold_short');
  var footswitchPresetSelectHoldLong = document.querySelectorAll('.select_footswitch_preset_hold_long');


  for (var i = 0; i < footswitchHeader.length; i++){
    footswitchHeader[i].innerHTML = "- FOOTSWITCH " + (i + 1) + " -";
    footswitchPresetSelectPressed[i].setAttribute('idx', i);
    footswitchPresetSelectPressed[i].onchange = onFootswitchChange;
    footswitchPresetSelectReleased[i].setAttribute('idx', i);
    footswitchPresetSelectReleased[i].onchange = onFootswitchChange;
    footswitchPresetSelectHoldShort[i].setAttribute('idx', i);
    footswitchPresetSelectHoldShort[i].onchange = onFootswitchChange;
    footswitchPresetSelectHoldLong[i].setAttribute('idx', i);
    footswitchPresetSelectHoldLong[i].onchange = onFootswitchChange;
  }
}

//Visablitiy functions
function setLoginVisable(){
    let x = document.getElementsByClassName("loginInvisable");
    if(x.length){
      x[0].className = "loginVisable";
    }
}
function setLoginInvisable(){
  let x = document.getElementsByClassName("loginVisable");
  if(x.length){
    x[0].className = "loginInvisable";
  }
}
function setPresetVisable(){
  let x = document.getElementsByClassName("presetInvisable");
  if(x.length){
    x[0].className = "presetVisable";
  }
}
function setPresetInvisable(){
  let x = document.getElementsByClassName("presetVisable");
  if(x.length){
    x[0].className = "presetInvisable";
  }
}
function setFootswitchVisable(){
  let x = document.getElementsByClassName("footswitchInvisable");
  if(x.length){
    x[0].className = "footswitchVisable";
  }
}
function setFootswitchInvisable(){
  let x = document.getElementsByClassName("footswitchVisable");
  if(x.length){
    x[0].className = "footswitchInvisable";
  }
}
function setLoaderVisable(){
  let x = document.getElementsByClassName("loaderInvisable");
  if(x.length){
    x[0].className = "loaderVisable";
  }
}
function setLoaderInvisable(){
  let x = document.getElementsByClassName("loaderVisable");
  if(x.length){
    x[0].className = "loaderInvisable";
  }
}

function loadHtml() {
    createStorage();
    insertNavigation();
    insertLogin();
    insertPreset();
    insertFootswitch();

    setLoginVisable();
    setPresetVisable();
    setFootswitchVisable();

    createPresetDropdown();
    updatePreset();
}

/******************************************
Helper
******************************************/
// Event listener to the form
sendForm.addEventListener('submit', function(event) {
  event.preventDefault();
  bt_send(inputField.value);
  inputField.focus();
});

let deviceCache = null;
let characteristicCache = null;
let readBuffer = '';

function bt_connect() {
  /* If device cache is already filled do promise, else request devices */
  return (deviceCache ? Promise.resolve(deviceCache) :
      bt_requestBluetoothDevice()).
      then(device => bt_connectDeviceAndCacheCharacteristic(device)).
      then(characteristic => bt_startNotifications(characteristic)).
      catch(error => log(error));
}

function bt_requestBluetoothDevice() {
  log('Requesting bluetooth device...');

  return navigator.bluetooth.requestDevice({
    filters: [{services: [0xFFE0]}],
  }).
      then(device => {
        log('"' + device.name + '" bluetooth device selected');
        deviceCache = device;
        deviceCache.addEventListener('gattserverdisconnected',
            bt_handleDisconnection);
        return deviceCache;
      });
}

function bt_handleDisconnection(event) {
  let device = event.target;

  log('"' + device.name +
      '" bluetooth device disconnected, trying to reconnect...');
  setLoaderVisable();
  bt_connectDeviceAndCacheCharacteristic(device).
      then(characteristic => bt_startNotifications(characteristic)).
      catch(error => bt_onDisconnection(error));
}

function bt_connectDeviceAndCacheCharacteristic(device) {
  setLoaderVisable();
  if (device.gatt.connected && characteristicCache) {
    return Promise.resolve(characteristicCache);
  }

  log('Connecting to GATT server...');

  return device.gatt.connect().
      then(server => {
        log('GATT server connected, getting service...');

        return server.getPrimaryService(0xFFE0);
      }).
      then(service => {
        log('Service found, getting characteristic...');

        return service.getCharacteristic(0xFFE1);
      }).
      then(characteristic => {
        log('Characteristic found');
        characteristicCache = characteristic;

        return characteristicCache;
      });
}

function bt_startNotifications(characteristic) {
  log('Starting notifications...');
  //Connected to a bt device
  bt_onConnection();
  return characteristic.startNotifications().
      then(() => {
        log('Notifications started');
        characteristic.addEventListener('characteristicvaluechanged',
            bt_handleCharacteristicValueChanged);
      });
}

function bt_handleCharacteristicValueChanged(event) {
  let value = new TextDecoder().decode(event.target.value);

  for (let c of value) {
    if (c === '\n') {
      let data = readBuffer.trim();
      readBuffer = '';

      if (data) {
        bt_receive(data);
      }
    }
    else {
      readBuffer += c;
    }
  }
}

function bt_receive(data) {
  log(data, 'in');
  //Here add the parser
}

function log(data, type = '') {
  console.log(data)
  // terminalContainer.insertAdjacentHTML('beforeend',
      // '<div' + (type ? ' class="' + type + '"' : '') + '>' + data + '</div>');
}

function bt_disconnect() {
  if (deviceCache) {
    log('Disconnecting from "' + deviceCache.name + '" bluetooth device...');
    deviceCache.removeEventListener('gattserverdisconnected',
        bt_handleDisconnection);

    if (deviceCache.gatt.connected) {
      deviceCache.gatt.disconnect();
      log('"' + deviceCache.name + '" bluetooth device disconnected');
    }
    else {
      log('"' + deviceCache.name +
          '" bluetooth device is already disconnected');
    }
  }

  if (characteristicCache) {
    characteristicCache.removeEventListener('characteristicvaluechanged',
        bt_handleCharacteristicValueChanged);
    characteristicCache = null;
  }

  deviceCache = null;
}

function bt_send(data) {
  data = String(data);
  if (!data || !characteristicCache) {
    return;
  }
  data += '\n';
  if (data.length > 20) {
    let chunks = data.match(/(.|[\r\n]){1,20}/g);
    bt_writeToCharacteristic(characteristicCache, chunks[0]);
    for (let i = 1; i < chunks.length; i++) {
      setTimeout(() => {
        bt_writeToCharacteristic(characteristicCache, chunks[i]);
      }, i * 100);
    }
  }
  else {
    bt_writeToCharacteristic(characteristicCache, data);
  }
  log(data, 'out');
}

function bt_writeToCharacteristic(characteristic, data) {
  characteristic.writeValue(new TextEncoder().encode(data));
}

function bt_onConnection(){
  setLoaderInvisable();
  setLoginInvisable();
  setFootswitchInvisable();
  setPresetVisable();
  //Shot BT icon
  let x = document.getElementsByClassName("nav_btImageInvisable");
  if(x.length){
    x[0].className = "nav_btImageVisable";
  }
}

function bt_onDisconnection(data){
  bt_disconnect();
  setLoaderInvisable();
  setLoginVisable();
  setFootswitchInvisable();
  setPresetInvisable();
  let x = document.getElementsByClassName("nav_btImageVisable");
  if(x.length){
    x[0].className = "nav_btImageInvisable";
  }
  console.log(data);
}
