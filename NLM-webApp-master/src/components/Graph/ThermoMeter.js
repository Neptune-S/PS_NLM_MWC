import React from "react";
import ReactCanvasGauge from "./ReactCanvus";

export default class ThermoMeter extends React.Component {
  constructor(props) {
    super(props);
    this.state = { value: 10 };
  }

  // componentDidMount() {
   
  //     const value1 = this.props.cell_snr_dB;

  //     this.setState({ value: value1 });
  
  //   // this.setState({ intervalHandler });
  // }

  // componentWillUnmount() {
  //   window.clearInterval(this.state.intervalHandler);
  // }

  changeColor = (data) => {
    if(data < 10){
      return "#FFA500"
    }else{
      return "#FFA500"
    }
  }
  
  render() {
    // console.log("sdsadfsafas", this.props);
    return (
        <ReactCanvasGauge
          key="linearGauge"
          width={125}
          height={275}
          minorTicks= {5}
          majorTicks= {[0,10,20,30,40]}
          value={Math.round((this.props?.data?.cell_snr_dB)/10)}
          gaugeType="LINEAR"
          colorBar=""
          colorBarProgress={this.changeColor(this.state.value)}
        />
    );
  }
}
