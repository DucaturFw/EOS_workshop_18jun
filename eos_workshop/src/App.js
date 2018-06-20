import React from "react";
import { BrowserRouter as Router, Route, Link } from "react-router-dom";
import Eos from 'eosjs';

const config = {
  keyProvider: '5Kat1ikjeVg29vB1yxN6vNLqQy5AogsUdKcehPrzJ9iX8iM2VSd',
  chainId: null, // 32 byte (64 char) hex string
  httpEndpoint: 'http://35.172.116.56:8888',
  expireInSeconds: 60,
  broadcast: true,
  verbose: false, // API activity
  sign: true
}

class EOSActionCall extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      config: props.config,
      type: props.type,
      action: 'getTableRows',
      data: '{"json": 1, "scope": "tester3", "code": "tester3", "table": "exoffer", "limit": 1}',
      actions: `[{
  "account": "eosio.token",
  "name": "transfer",
  "authorization": [{
    "actor": "tester3",
    "permission": "active"
  }],
  "data": {
    "from": "tester3",
    "to": "tester1",
    "quantity": "7.0000 SYS",
    "memo": ""
  }
}]`
    }
    
    this.handleInputChange = this.handleInputChange.bind(this);
    this.sendTransaction = this.sendTransaction.bind(this);
  }

  componentWillReceiveProps(props) {
    this.setState(state => ({
      ...state,
      ...props
    }))
  }

  sendTransaction() {
    const eos = Eos(this.state.config);
    if (this.state.type == 'api') {
      const action = this.state.action, data = JSON.parse(this.state.data);
      eos[action](data).then(res => {
        this.setState({'res': JSON.stringify(res, null, '\t')});
        console.log(res);
      }).catch(res => {
        this.setState({'res': JSON.stringify(res, null, '\t')});
        console.error(res);
      })
    } else {
      const actions = JSON.parse(this.state.actions);
      eos.transaction(
        {
          // ...headers,
          actions: actions,
        }
        // options -- example: {broadcast: false}
      ).then(res => {
        this.setState({'res': JSON.stringify(res, null, '\t')});
        console.log(res);
      }).catch(res => {
        this.setState({'res': JSON.stringify(res, null, '\t')});
        console.error(res);
      })
    }
  }

  handleInputChange(event) {
    const target = event.target;
    const value = target.type === 'checkbox' ? target.checked : target.value;
    const name = target.name;

    this.setState({
      [name]: value
    });
  }

  render() {
    let color = '#fff3dd';
    if (this.state.type == 'api') color = '#e8f0ff';
    return (
      <div style={{background: color, padding: '10px'}}>

        {this.state.type === 'api' && 
          <form>
            <div style={{margin: '15px', marginBottom: '1em'}}>
              <label>API Method:
                <input style={{fontSize: '12pt'}} type={"text"} onChange={this.handleInputChange} name="action"
                    value={this.state.action} />
              </label>
            </div>
            <div style={{margin: '15px', marginBottom: '1em'}}>
              <label>Data:
                <textarea style={{fontSize: '12pt'}} type={"text"} onChange={this.handleInputChange} name="data"
                    value={this.state.data} />
              </label>
            </div>
          </form>
        }

        {this.state.type !== 'api' && 
          <form>
            <div style={{margin: '15px', marginBottom: '1em'}}>
              <label>Actions:
                <textarea style={{fontSize: '12pt'}} type={"text"} onChange={this.handleInputChange} name="actions"
                    value={this.state.actions} />
              </label>
            </div>
          </form>
        }

        <button onClick={this.sendTransaction}>Send transaction and await</button>
        <br />
        <p style={{wordWrap: "break-word"}}>{ this.state.res }</p>
      </div>
    )
  }
}

class EOSConfig extends React.Component {
  constructor(props) {
    super(props);
    this.state = config;

    this.handleInputChange = this.handleInputChange.bind(this);
  }

  handleInputChange(event) {
    const target = event.target;
    const value = target.type === 'checkbox' ? target.checked : target.value;
    const name = target.name;

    this.setState({
      [name]: value
    });
  }

  render() {
    return <div style={{fontSize: '14pt'}}>
      <h2>EOS</h2>
      <div style={{padding: '15px', bgcolor: 'aqua'}}>Current configuration:<br />{JSON.stringify(this.state, null, 2)}</div>

      <div style={{margin: '15px', marginBottom: '1em'}}>
        <label>Key provider:
          <input style={{fontSize: '14pt'}} size="80" type={"text"} onChange={this.handleInputChange} name="keyProvider"
              value={this.state.keyProvider} />
        </label>
      </div>

      <div style={{margin: '15px', marginBottom: '1em'}}>
        <label>HTTP Endpoint:
          <input style={{fontSize: '14pt'}} size="80" type={"text"} onChange={this.handleInputChange} name="httpEndpoint"
              value={this.state.httpEndpoint} />
        </label>
      </div>

      <div>
        <div style={{width: '50%', float: 'left'}}>
          <EOSActionCall config={this.state} type='api' />
        </div>
        <div style={{width: '50%', float: 'left'}}>
          <EOSActionCall config={this.state} />
        </div>
      </div>
    </div>
  }
}

const Scatter = ({ match }) => (
  <div>
    <h2>Topics</h2>
    <ul>
      <li>
        <Link to={`${match.url}/eos`}>EOS</Link>
      </li>
      <li>
        <Link to={`${match.url}/eth`}>ETH</Link>
      </li>
    </ul>

    <Route path={`${match.url}/:topicId`} component={Topic} />
    <Route
      exact
      path={match.url}
      render={() => <h3>Please select a topic.</h3>}
    />
  </div>
);

const Topic = ({ match }) => (
  <div>
    <h3>{match.params.topicId}</h3>
  </div>
);

const App = () => (
  <Router>
    <div>
      <ul>
        <li>
          <Link to="/">EOS pure</Link>
        </li>
        <li>
          <Link to="/scatter">Scatter</Link>
        </li>
      </ul>

      <hr />

      <Route exact path="/" component={EOSConfig} />
      <Route path="/scatter" component={Scatter} />
    </div>
  </Router>
);

export default App;