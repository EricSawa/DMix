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
// TODO: Send load config command
// TODO: Add wait after sending command

function onPageLoad() {
    insertNavigation();
    insertLogin();
    insertPreset();
    insertFootswitch();

    createStorage();
    createPresetDropdown();
    updatePreset();
    
    setLoaderInvisable();
    setNavigationVisable();
    setLoginVisable();
    setPresetVisable();
    setFootswitchVisable();

    protocoll_parseFootswitch("#F0000102030A0\r");
    protocoll_parseFootswitch("#F001010203040\r");

    protocoll_parseTransition("#T00101ff03000\r");
    protocoll_parseTransition("#T101017fffff0\r");
    protocoll_parseTransition("#T201017fff7f1\r");

    protocoll_parsePresetName("#P001EricPrese\r");
    protocoll_parsePresetName("#P101t123     \r");

    protocoll_sendLoadConfig();
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
      if(this.checked) {
        value = 1;
      }
      else {
        value = 0;
      }
  }else{
    if(value > max) {
      this.value = max;
    }else if(value < min){
        this.value = min;
    }
    value = this.value;
  }
  //Load values into storage
  _dictPreset[_activePreset]["Params"][idx][key] = value;
  protocoll_sendTransition(_activePreset, idx);
  console.log(_dictPreset[_activePreset]["Params"][idx]);
}

function onPresetNumberChange(){
  //Change presetNumber
  _activePreset = this.value;
  updatePreset();
}

function onPresetNameChange(){
  presetName = this.value;
  _dictPreset[_activePreset].Name = presetName;
  protocoll_sendPresetName(_activePreset);
  updatePresetDropdownContent();
}

function onFootswitchChange(){
  let key = this.getAttribute('key');
  let idx = this.getAttribute('idx');
  let value = this.value;
  //Load values into storage
  _dictFootswitch[idx][key] = value;
  protocoll_sendFootswitch(idx);
  //console.log(_dictFootswitch[idx]);
}

function onFootswitchTest(){
  let key = this.getAttribute('key');
  let idx = this.getAttribute('idx');
  protocoll_sendPresetTrigger(_dictFootswitch[idx][key])
  //console.log(_dictFootswitch[idx]);
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

function updateFootswitch(){
  //Update Footswitch dropdown
  let pressedOptions = document.querySelectorAll('.select_footswitch_preset_pressed');
  let releasedOptions = document.querySelectorAll('.select_footswitch_preset_released');
  let holdShortOptions = document.querySelectorAll('.select_footswitch_preset_hold_short');
  let holdLongOptions = document.querySelectorAll('.select_footswitch_preset_hold_long');
  for(let i = 0; i < pressedOptions.length; ++i){
    pressedOptions[i].selectedIndex = _dictFootswitch[i]["pressed"];
    releasedOptions[i].selectedIndex =_dictFootswitch[i]["released"];
    holdShortOptions[i].selectedIndex = _dictFootswitch[i]["hold_short"];
    holdLongOptions[i].selectedIndex = _dictFootswitch[i]["hold_long"];
  }
}
/******************************************
On connection
******************************************/
function bt_onConnection(){
  setLoaderInvisable();
  setLoginInvisable();
  setFootswitchInvisable();
  setPresetVisable();
  setConIconVisable("bt");
  setConIconInvisable("usb");
}

function bt_onDisconnection(data){
  bt_disconnect();
  setLoaderInvisable();
  setLoginVisable();
  setFootswitchInvisable();
  setPresetInvisable();
  setConIconInvisable("bt");
  console.log(data);
}
/******************************************
Protocoll
******************************************/
/*Footswitch messages*/
function protocoll_sendFootswitch(fsNr){
    let pressed = _dictFootswitch[fsNr]["pressed"];
    let released = _dictFootswitch[fsNr]["released"];
    let hold_short = _dictFootswitch[fsNr]["hold_short"];
    let hold_long = _dictFootswitch[fsNr]["hold_long"];
    let cmdStr1 = "#F0"+byteToHex(fsNr)+byteToHex(pressed)+byteToHex(released)+byteToHex(hold_short)+byteToHex(hold_long)+"0\r";
    //TODO: Send via interface
    console.log(cmdStr1);
}
function protocoll_parseFootswitch(command){
  let header = command.substring(0, 3);
  if(header == "#F0"){
    let fsNr = parseInt(command.substring(3, 5),16);
    _dictFootswitch[fsNr]["pressed"] = protocoll_clamp(parseInt(command.substring(5, 7),16), 0, (numberPresets-1));
    _dictFootswitch[fsNr]["released"] = protocoll_clamp(parseInt(command.substring(7, 9),16), 0, (numberPresets-1));
    _dictFootswitch[fsNr]["hold_short"] = protocoll_clamp(parseInt(command.substring(9, 11),16), 0, (numberPresets-1));
    _dictFootswitch[fsNr]["hold_long"] = protocoll_clamp(parseInt(command.substring(11, 13),16), 0, (numberPresets-1));
  }
  updateFootswitch();
}
/*Transition messages*/
function protocoll_sendTransition(prNr, trNr){
  let ch = _dictPreset[prNr]["Params"][trNr]["channel"];
  let start = _dictPreset[prNr]["Params"][trNr]["startValue"];
  let end = _dictPreset[prNr]["Params"][trNr]["endValue"];
  let time = _dictPreset[prNr]["Params"][trNr]["transitionTime"];
  let delay = _dictPreset[prNr]["Params"][trNr]["delay"];
  let ramp = _dictPreset[prNr]["Params"][trNr]["ramp"];
  let active = _dictPreset[prNr]["Params"][trNr]["active"];
  let cmdStr1 = "#T0"+byteToHex(prNr)+byteToHex(trNr)+byteToHex(ch)+byteToHex(start)+"000\r";
  let cmdStr2 = "#T1"+byteToHex(prNr)+byteToHex(trNr)+byteToHex(end)+shortToHex(time)+"0\r";
  let cmdStr3 = "#T2"+byteToHex(prNr)+byteToHex(trNr)+shortToHex(delay)+byteToHex(ramp)+nibbleToHex(active)+"\r";
  //TODO: Send via interface
  console.log(cmdStr1);
  console.log(cmdStr2);
  console.log(cmdStr3);
}
function protocoll_parseTransition(command){
  let header = command.substring(0, 3);
  let prNr = parseInt(command.substring(3, 5),16);
  let trNr = parseInt(command.substring(5, 7),16);
  if(header == "#T0"){
    _dictPreset[prNr]["Params"][trNr]["channel"] = protocoll_clamp(parseInt(command.substring(7, 9),16), 0, 255);
    _dictPreset[prNr]["Params"][trNr]["startValue"] = protocoll_clamp(parseInt(command.substring(9, 11),16), 0, 255);
  }else if (header == "#T1"){
    _dictPreset[prNr]["Params"][trNr]["endValue"] = protocoll_clamp(parseInt(command.substring(7, 9),16), 0, 255);
    _dictPreset[prNr]["Params"][trNr]["transitionTime"] = protocoll_clamp(parseInt(command.substring(9, 13),16), 0, 65532);
  }else if (header == "#T2"){
    _dictPreset[prNr]["Params"][trNr]["delay"] = protocoll_clamp(parseInt(command.substring(7, 11),16), 0, 65532);
    _dictPreset[prNr]["Params"][trNr]["ramp"] = protocoll_clamp(parseInt(command.substring(11, 13),16), 0, 0);
    _dictPreset[prNr]["Params"][trNr]["active"] = protocoll_clamp(parseInt(command.substring(13, 14),16), 0, 1);
  }
  updatePreset();
}

/*Preset messages*/
function protocoll_sendPresetTrigger(prNr){
    let cmdStr1 = "#PT"+byteToHex(prNr)+"000000000\r";
    //TODO: Send via interface
    console.log(cmdStr1);
}
function protocoll_sendPresetName(prNr){
  let stringName = _dictPreset[prNr].Name.padEnd(18,' ');
  let cmdStr1 = "#P0" + byteToHex(prNr) + stringName.substring(0, 9) + "\r";
  let cmdStr2 = "#P1" + byteToHex(prNr) + stringName.substring(9, 18) + "\r";
  //TODO: Send via interface
  console.log(cmdStr1);
  console.log(cmdStr2);
}
function protocoll_parsePresetName(command){
  let header = command.substring(0, 3);
  let prNr = parseInt(command.substring(3, 5),16);
  if(header == "#P0"){
    _dictPreset[prNr].Name = "";
    _dictPreset[prNr].Name = command.substring(5, 14);
  }else if(header == "#P1"){
    let newString = _dictPreset[prNr].Name.substring(0, 9);
    _dictPreset[prNr].Name = newString + command.substring(5, 14);
  }
  updatePreset();
}
/*Load configuration messages*/
function protocoll_sendLoadConfig(){
    let cmdStr1 = "#L0"+"00000000000\r";
    //TODO: Send via interface
    console.log(cmdStr1);
}
/*Protocoll helper*/
function protocoll_clamp(num, min, max){
  return num <= min ? min : num >= max ? max : num;
}
/******************************************
Helper
******************************************/
var nibbleToHex = function (byte) {
  var hex = Number(byte).toString(16);
  return hex;
};
var byteToHex = function (byte) {
  var hex = Number(byte).toString(16);
  while (hex.length < 2) {
       hex = "0" + hex;
  }
  return hex;
};
var shortToHex = function (short) {
  var hex = Number(short).toString(16);
  while (hex.length < 4) {
       hex = "0" + hex;
  }
  return hex;
};


/******************************************
Bluetooth Logic
******************************************/
// Event listener to the form
// sendForm.addEventListener('submit', function(event) {
//   event.preventDefault();
//   bt_send(inputField.value);
//   inputField.focus();
// });

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
/******************************************
HTML Visability
******************************************/
function setPresetPageVisable(){
  setLoginInvisable();
  setFootswitchInvisable();
  setPresetVisable();
}

function setFootswitchPageVisable(){
  setLoginInvisable();
  setPresetInvisable();
  setFootswitchVisable();
}

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
  let y = document.getElementsByClassName("grey_overlayInvisable");
  if(x.length){
    x[0].className = "loaderVisable";
    y[0].className = "grey_overlayVisable";
  }
}
function setLoaderInvisable(){
  let x = document.getElementsByClassName("loaderVisable");
  let y = document.getElementsByClassName("grey_overlayVisable");
  if(x.length){
    x[0].className = "loaderInvisable";
    y[0].className = "grey_overlayInvisable";
  }
}

function setNavigationVisable(){
  let x = document.getElementsByClassName("nav_colInvisable");
  if(x.length){
    x[0].className = "nav_colVisable";
  }
}
function setNavigationInvisable(){
  let x = document.getElementsByClassName("nav_colVisable");
  if(x.length){
    x[0].className = "nav_colInvisable";
  }
}
function setConIconVisable(icon){
  if(icon == "bt"){
    let x = document.getElementsByClassName("nav_btImageInvisable");
    if(x.length){
      x[0].className = "nav_btImageVisable";
    }
  }
  else if (icon == "usb") {
    let x = document.getElementsByClassName("nav_usbImageInvisable");
    if(x.length){
      x[0].className = "nav_usbImageVisable";
    }
  }
}

function setConIconInvisable(icon){
  if(icon == "bt"){
    let x = document.getElementsByClassName("nav_btImageVisable");
    if(x.length){
      x[0].className = "nav_btImageInvisable";
    }
  }
  else if (icon == "usb") {
    let x = document.getElementsByClassName("nav_usbImageVisable");
    if(x.length){
      x[0].className = "nav_usbImageInvisable";
    }
  }
}

/******************************************
HTML Elemente
******************************************/
// Navigation Bar
var navigationHtml = [
  '<div class="loaderInvisable"></div>',
  '<div class="grey_overlayInvisable"></div>',
  '<div class="navigation w-row">',
    '<div class ="nav_col1 ">',
      '<div class="nav_col1text">DMiX</div>',
    '</div>',
    '<div class="nav_colVisable">',
      '<div class="nav_colConTo">',
        '<div class="nav_col2text">Connected: </div>',
      '</div>',
      '<div class="nav_colCon">',
        '<img src="images/usb.png" alt="" class="nav_usbImageInvisable" >',
        '<img src="images/bt.png" alt="" class="nav_btImageVisable" onclick="bt_onDisconnection()">',
      '</div>',
      '<div class="nav_colSpace"></div>',
      '<div class="nav_colPreset" onclick="setPresetPageVisable()">',
        '<div class="text_nav">PRESET</div>',
      '</div>',
      '<div class="nav_colFootswitch" onclick="setFootswitchPageVisable()">',
        '<div class="text_nav">FOOTSWITCH</div>',
      '</div>',
    '</div>',
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
            '<input class="input_prname" type="text" placeholder="Example Preset" maxlength="18">',
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
        '<button class="btn_footswitch_preset_pressed" type="button" key="pressed">TEST</button>',
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
        '<button class="btn_footswitch_preset_released" type="button" key="released">TEST</button>',
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
        '<button class="btn_footswitch_preset_hold_short" type="button" key="hold_short">TEST</button>',
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
        '<button class="btn_footswitch_preset_hold_long" type="button" key="hold_long">TEST</button>',
      '</div>',
    '</div>',
  '</div>',
].join("\n");
/******************************************
Generate Page and Storage
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
  updatePresetDropdownContent();
  //Load last NAME
  presetName[0].value = _dictPreset[_activePreset].Name;
}

function updatePresetDropdownContent(){
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
/* Generate Page */
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
  var footswitchTestPressed = document.querySelectorAll('.btn_footswitch_preset_pressed');
  var footswitchTestReleased = document.querySelectorAll('.btn_footswitch_preset_released');
  var footswitchTestHoldShort = document.querySelectorAll('.btn_footswitch_preset_hold_short');
  var footswitchTestHoldLong = document.querySelectorAll('.btn_footswitch_preset_hold_long');
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
    footswitchTestPressed[i].setAttribute('idx', i);
    footswitchTestReleased[i].setAttribute('idx', i);
    footswitchTestHoldShort[i].setAttribute('idx', i);
    footswitchTestHoldLong[i].setAttribute('idx', i);
    footswitchTestPressed[i].onclick = onFootswitchTest;
    footswitchTestReleased[i].onclick = onFootswitchTest;
    footswitchTestHoldShort[i].onclick = onFootswitchTest;
    footswitchTestHoldLong[i].onclick = onFootswitchTest;
  }
}
