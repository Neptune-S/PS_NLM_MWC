import React, { useEffect, useState } from "react";
import LiquidChart from "./LiquidChart";
import ThermoMeter from "./ThermoMeter";
import "./graph.css";
import ReactRadialGauge from "./SpeedoMeter";
import Button from "react-bootstrap/Button";
import Modal from "react-bootstrap/Modal";
import "../../../node_modules/bootstrap/dist/css/bootstrap.min.css";

// import { useWebSocket } from "react-use-websocket/dist/lib/use-websocket";
// const WS_URL = 'ws://192.168.3.75:3000';

export default function Graph({ handleClick,getParentData }) {
  const [show, setShow] = useState(false);
  const [gscn, setGscn] = useState("");
  const [subscription, setsubscription] = useState("");
  const [buttonName, setbuttonName] = useState("")
  const [data, setData] = useState({ cell_rssi_dBm: -1200, cell_snr_dB: 0, cfo_ppb: 0 });

  const handleClose = () => {
    setShow(false)
  };
  const handleShow1 = () => {
    setShow(true)
    setbuttonName("cellFello")

  };

  const handleShow2 = () => {
    setShow(true)
    setbuttonName("cellFellMib")
  };

  function handleSubmit() {
    if (buttonName === "cellFello") {
      //setsubscription(`\n{\"msg_type\":1,\"gscn\":${gscn}}`)
      setsubscription(1)
     setShow(true)
     ws.send(1);
    } else {
      //setsubscription(`\n{\"msg_type\":2,\"gscn\":${gscn}}`)
      setsubscription(2)
      setShow(true)
      ws.send(2);
    }

    handleClose()
  }


  const handleChange = (event) => {
    // setsubscription({msg_type:"1",gscn:event.target.value })
    setGscn(event.target.value);
  };
  const bandInfo = () => {
    //setsubscription({ msg_type: 9, gscn: 0 })
    setsubscription(0)
    ws.send(0);
    console.log("msg_type:0", gscn);
  }
  const bandStop = () => {
    //setsubscription({ msg_type: 3, gscn: 0 })
    getParentData("")
    setData({ cell_rssi_dBm: -1200, cell_snr_dB: 0, cfo_ppb: 0 })
    setsubscription(3)
    handleClick()
    ws.send(3);
    console.log("msg_type:3", data)
  }


  const ws = new WebSocket(
    "ws://127.0.0.1:8443"
  );

 
    ws.onopen = () => {
      // ws.send(subscription)
      console.log("Connection Established!");
      console.log(subscription);    
    };
    ws.onmessage = (event) => {
      const response = JSON.parse(event.data);
      console.log('response', response);
      getParentData(response)
      setData(response)
    };
  
  
  

  // useEffect(() => {
  //  initwebsocket()
  // }, [data]);


  // useWebSocket(WS_URL, {
  //   onOpen: () => {
  //     console.log('WebSocket connection established.');
  //   }
  // });

  return (
    <>
      <Modal show={show} onHide={handleClose}>
        <Modal.Header closeButton>
          <Modal.Title>Enter GSCN</Modal.Title>
        </Modal.Header>
        <Modal.Body>
          <form>
            <label>
              GSCN :
              <input type="text" value={gscn} name="gscn" placeholder="Enter value" onChange={handleChange} />
            </label>
          </form>
        </Modal.Body>
        <Modal.Footer>

          <Button variant="primary" onClick={handleSubmit} >
            Submit
          </Button>
        </Modal.Footer>
      </Modal>
      <div className="row1">
        <div className="column1">
          <div className="card1">
            <div className="pumpa">
              <div className="graphhead">
                RSSI(dBm)
                <LiquidChart data={data} />
              </div>
            </div>
          </div>
        </div>
        <div className="column1">
          <div className="card1">
            <div className="pumpa">
              <div className="graphhead">
                SNR(dB)
                <ThermoMeter data={data} />
              </div>
            </div>
          </div>
        </div>
        <div className="column1">
          <div className="card1">
            <div className="pumpa">
              <div className="graphhead">
                CFO(ppb)
                <ReactRadialGauge data={data} />
              </div>
            </div>
          </div>
        </div>
      </div>
      <div className="btnfeature">
        <h4 className="btnheadf">Features</h4>
        <Button className="btnhead btn-info" onClick={bandInfo}>Band Scan(n77)</Button>
        <Button className="btnhead" variant="primary" onClick={handleShow1}>
          Cell Follow
        </Button>
        <Button className="btnhead" variant="primary" onClick={handleShow2}>
          Cell Follow MIB
        </Button>
        <Button className="btnhead btn-danger" onClick={bandStop}>Cell Follow Stop</Button>
      </div>
    </>
  );
}