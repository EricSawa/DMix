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
let dictTransition = [];
let dictPreset = [];
let dictFootswitch = [];
let activePreset = 0;
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
          '<select class="select_transition_ramp"  key="ramp">',
            '<option value="linear">LIN</option>',
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
          '<select class="select_footswitch_preset"  key="preset">',
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
          '<select class="select_footswitch_preset"  key="preset">',
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
          '<select class="select_footswitch_preset"  key="preset">',
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
          '<select class="select_footswitch_preset"  key="preset">',
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
  //Generate transition dictionary
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
    //Generate preset array
    for(var j = 0; j < numberPresets; ++j){
      dictPreset.push({
        "Name": "Preset " + j,
        "Params": dictTransition
      });
    }
    //Generate footswitch array
    for(var j = 0; j < numberFootswitch; ++j){
      dictFootswitch.push({
        "released": 255,
        "pressed": 255,
        "hold_short": 255,
        "hold_long": 255,
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
  presetNumberOptions = document.querySelectorAll('.select_footswitch_preset');
  for(let i = 0; i < presetNumberOptions.length; ++i){
    for(let j = 0; j < numberPresets; ++j){
      // create new option element
      let opt = document.createElement('option');
      opt.value = j;
      presetNumberOptions[i].add(opt);
    }
  }
  //Update dropdown list
  updatePresetDropdown();
  //Load last NAME
  presetName[0].value = dictPreset[activePreset].Name;
}

function updatePresetDropdown(){
  //Update PRESETS dropdown
  let presetNumberOptions = document.querySelectorAll('.select_prnmbr_dropdown');
  let options = presetNumberOptions[0].options;
  for(let i = 0; i < options.length; ++i){
    options[i].text = i + " - [" + dictPreset[i].Name + "]";
  }
  //Update Footswitch dropdown
  presetNumberOptions = document.querySelectorAll('.select_footswitch_preset');
  for(let i = 0; i < presetNumberOptions.length; ++i){
    options = presetNumberOptions[i].options;
    for(let j = 0; j < options.length; ++j){
      options[j].text = j + " - [" + dictPreset[j].Name + "]";
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
  console.log("transition:" + idx + " key:" + key  + " value:" + value);
}

function onPresetNumberChange(){
  activePreset = this.value;
  let presetName = document.querySelectorAll('.input_prname');
  console.log("preset: " + activePreset + " name: " + dictPreset[activePreset].Name);
  presetName[0].value = dictPreset[activePreset].Name;
}
function onPresetNameChange(){
  presetName = this.value;
  dictPreset[activePreset].Name = presetName;
  console.log("preset: " + activePreset + " name: " + dictPreset[activePreset].Name);
  updatePresetDropdown();
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
function loadPreset(presetNumber){
  var transitionChannel = document.querySelectorAll('.input_transition_ch');
  var transitionStart = document.querySelectorAll('.input_transition_start');
  var transitionEnd = document.querySelectorAll('.input_transition_end');
  var transitionTime = document.querySelectorAll('.input_transition_time');
  var transitionDelay = document.querySelectorAll('.input_transition_delay');
  var transitionRamp = document.querySelectorAll('.select_transition_ramp');
  var transitionActive = document.querySelectorAll('.checkbox_transition_active');
  for (var i = 0; i < transitionChannel.length; i++){
    transitionChannel[i].setAttribute('preset', presetNumber);
    transitionStart[i].setAttribute('preset', presetNumber);
    transitionEnd[i].setAttribute('preset', presetNumber);
    transitionTime[i].setAttribute('preset', presetNumber);
    transitionDelay[i].setAttribute('preset', presetNumber);
    transitionRamp[i].setAttribute('preset', presetNumber);
    transitionActive[i].setAttribute('preset', presetNumber);
  }
}

function insertFootswitch(){
  document.body.insertAdjacentHTML( 'beforeend', footswitchHtml );
  var x = document.getElementsByClassName("div_footswitch");
  for (var i = 0; i < numberFootswitch; i++){
    x[0].insertAdjacentHTML('beforeend', footswitchParamsHtml );
  }
  var footswitchHeader = document.querySelectorAll('.div_footswitch_param > .heading-1');
  // var transitionChannel = document.querySelectorAll('.input_transition_ch');
  // var transitionStart = document.querySelectorAll('.input_transition_start');
  // var transitionEnd = document.querySelectorAll('.input_transition_end');
  // var transitionTime = document.querySelectorAll('.input_transition_time');
  // var transitionDelay = document.querySelectorAll('.input_transition_delay');
  // var transitionRamp = document.querySelectorAll('.select_transition_ramp');
  // var transitionActive = document.querySelectorAll('.checkbox_transition_active');
  for (var i = 0; i < footswitchHeader.length; i++){
    //Count up the transisiton number
    footswitchHeader[i].innerHTML = "- FOOTSWITCH " + (i + 1) + " -";
  //  transitionChannel[i].setAttribute('idx', i);
    // transitionChannel[i].onchange = onTransitionChange;
    // transitionStart[i].setAttribute('idx', i);
    // transitionStart[i].onchange = onTransitionChange;
    // transitionEnd[i].setAttribute('idx', i);
    // transitionEnd[i].onchange = onTransitionChange;
    // transitionTime[i].setAttribute('idx', i);
    // transitionTime[i].onchange = onTransitionChange;
    // transitionDelay[i].setAttribute('idx', i);
    // transitionDelay[i].onchange = onTransitionChange;
    // transitionRamp[i].setAttribute('idx', i);
    // transitionRamp[i].onchange = onTransitionChange;
    // transitionActive[i].setAttribute('idx', i);
    // transitionActive[i].onchange = onTransitionChange;
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
    console.log(dictPreset);
    console.log(dictFootswitch);
    insertNavigation();
    insertLogin();
    insertPreset();
    insertFootswitch();
    loadPreset(2);
    setLoginVisable();
    setPresetVisable();
    setFootswitchVisable();

    createPresetDropdown();
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
