import { RadialGauge, LinearGauge } from "canvas-gauges";
import React from "react";

export default class ReactCanvasGauge extends React.Component {
  componentDidMount() {
    const options = { ...this.props, renderTo: this.canvasRef };
    switch (this.props.gaugeType) {
      case "LINEAR":
        this.gauge = new LinearGauge(options).draw();
        break;
      case "RADIAL":
        this.gauge = new RadialGauge(options).draw();
        break;
      default:
        break;
    }
  }

  componentWillReceiveProps(props) {
    if (this.gauge) {
      // console.log("hello",props.value);
      this.gauge.update({ value: props.value });
    }
  }
  render() {
    return <canvas ref={(node) => (this.canvasRef = node)}/>;
  }
}

