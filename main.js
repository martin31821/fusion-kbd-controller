let cachedElems = localStorage.getItem("keyboarddesign");
let cachedObject = {};
let currentNode = null;
if (!!cachedElems) {
  cachedObject = JSON.parse(cachedElems);
}

const dlfile = (content, filename) => {
  let contentType = 'application/octet-stream';
  var a = document.createElement('a');
  var blob = new Blob([content], {'type':contentType});
  a.href = window.URL.createObjectURL(blob);
  a.download = filename;
  a.click();
};

const validateAndSet = () => {
  if (!(/^\#[0-9a-f]{6,6}$/gi.test(inputElem.value))) {
    inputElem.style["box-shadow"] = "0px 0px 40px 0px red";
  } else {
    inputElem.style["box-shadow"] = "0px 0px 40px 0px green";
    if (!currentNode) {
      return;
    }
    cachedObject[currentNode.dataset.keyid] = inputElem.value;
    currentNode.style["background-color"] = inputElem.value;
    localStorage.setItem("keyboarddesign", JSON.stringify(cachedObject));
  };
};

inputElem = document.getElementById("color");
inputElem.onkeypress = validateAndSet;

for (let node of document.querySelectorAll(".key")) {
  if (cachedObject.hasOwnProperty(node.dataset.keyid)) {
    node.style["background-color"] = cachedObject[node.dataset.keyid];
  }
  node.onclick = () => {
    if (!!currentNode) {
      currentNode.style["box-shadow"] = "";
    }
    currentNode = node;
    currentNode.style["box-shadow"] = "0px 0px 20px 0px black";
    validateAndSet();
  }
}

dlbtn = document.querySelector(".dlbtn")
dlbtn.onclick = () => {
  // TODO: generate array buffer
  const buf = new ArrayBuffer(512);
  const view = new Uint8Array(buf);
  for (let i = 0; i < 512; i++) {
    view[i] = 0;
  }
  for (let node of document.querySelectorAll(".key")) {
    const data = cachedObject[node.dataset.keyid];
    if (!data) {
      continue;
    }
    const offs = (node.dataset.keyid - 1) * 4;
    view[offs+1] = parseInt(data.substr(1, 2), 16);
    view[offs+2] = parseInt(data.substr(3, 2), 16);
    view[offs+3] = parseInt(data.substr(5, 2), 16);
  }
  dlfile(view, "keyboard.config");
}
