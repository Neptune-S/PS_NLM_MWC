import React, { useEffect, useState } from "react";
import "./heading.css";
import files from "../../assets/images/arttha5G.png";
import logo from "../../assets/images/logo.svg";
import Graph from "../Graph/Graph";

export default function Heading() {
  const [getdata, setGetdata] = useState("")
  // console.log("getdatagetdata", getdata)

  const [active, setActive] = useState(true);
  const handleClick = () => {
    setActive(!active);
  };

  useEffect(()=>{
    if (getdata.cell_tracking === 1) {
    setActive(false)
    }else{
      setActive(true)
    }
   },[getdata])

  const getParentData = (para) => {
    setGetdata(para)
  }

  return (
    <>
      <div className="navh">
        <img src={files} className="imageh" alt="files" />
        <img src={logo} className="imageh" alt="logo" />
      </div>
      <hr />
      <h3 className="secondheadh">Network Listening Module 5G</h3>
      <button className="btntext">
        <label>NLM State:</label>
        {active === false ? <label className="labeltrack">Tracking</label> : <label className="labelstop">Stopped</label>}
      </button>
      <div className="rowh">
        <div className="columnh">
          <div className="cardh">
            <h4 className="textv fw-bold">CELL INFO</h4>
            <div className="textleft">
              <p className="paraspaceh">Cell ID:<span className="fw-bold">{getdata?.cell_follow_cell_id}</span></p>
              {/* <p className="paraspaceh">Rssi:  {Math.round((getdata.cell_rssi_dBm) / 10)}dBm</p>
              <p className="paraspaceh">SNR: {Math.round((getdata.cell_snr_dB) / 10)}dB</p> */}
              <p className="paraspaceh">SSB Transmitted: <span className="fw-bold">{getdata?.n_rx_ssb}</span></p>
              <p className="paraspaceh">False Alarm : <span className="fw-bold">{getdata?.n_false_alarm}</span></p>
              <p className="paraspaceh">SSB Missed:  <span className="fw-bold">{getdata?.n_missed_detection}</span></p>
              <p className="paraspaceh">Mib Failed :  <span className="fw-bold">{getdata?.n_mib_crc_fail}</span></p>
              <p className="paraspaceh">Cell Tracking:  {(() => {
                if (getdata.cell_tracking === 1) {

                  return (
                    <span className="fw-bold">Tracking</span>
                  )
                }
                else if (getdata.cell_tracking === 0) {

                  return (
                    <span className="fw-bold">sync lost</span>
                  )
                }
                return null;
              })()}</p>
            </div>
          </div>
        </div>
        <div className="columnh">
          <div className="cardh">
            <h5 className="textv fw-bold">MIB INFO</h5>
            <div className="textleft">
              <p className="paraspaceh">ssb_subcarrierOffset: <span className="fw-bold">{getdata.ssb_subcarrierOffset}</span></p>
              <p className="paraspaceh">pdcch_configSIB1: <span className="fw-bold">{getdata.pdcch_configSIB1}</span></p>
              <p className="paraspaceh">dmrs_typeA_Position:  {(() => {
                if (getdata.dmrs_typeA_Position === 1) {
                  return (
                    <span className="fw-bold">pos3</span>
                  )
                }
                else if (getdata.dmrs_typeA_Position === 0) {
                  return (
                    <span className="fw-bold">pos2</span>
                  )
                }
                return null;
              })()}</p>

              <p className="paraspaceh">cellBarred:  {(() => {
                if (getdata.cellBarred === 1) {
                  return (
                    <span className="fw-bold">notBarred</span>
                  )
                }
                else if (getdata.dmrs_typeA_Position === 0) {
                  return (
                    <span className="fw-bold">Barred</span>
                  )
                }
                return null;
              })()}</p>

              <p className="paraspaceh">intraFreqReselection:  {(() => {
                if (getdata.intraFreqReselection === 1) {
                  return (
                    <span className="fw-bold">notAllowed</span>
                  )
                }
                else if (getdata.dmrs_typeA_Position === 0) {
                  return (
                    <span className="fw-bold">Allowed</span>
                  )
                }
                return null;
              })()}</p>

              <p className="paraspaceh">subCarrierSpacingCommon:  {(() => {
                if (getdata.subCarrierSpacingCommon === 1) {
                  return (
                    <span className="fw-bold">scs30</span>
                  )
                }
                else if (getdata.dmrs_typeA_Position === 0) {
                  return (
                    <span className="fw-bold">scs15</span>
                  )
                }
                return null;
              })()}</p>
            </div>
          </div>
        </div>
      </div>
      <Graph handleClick={handleClick} getParentData={getParentData} />
    </>
  );
}
