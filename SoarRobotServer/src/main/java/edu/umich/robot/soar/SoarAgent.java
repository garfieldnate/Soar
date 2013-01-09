/*
 * Copyright (c) 2011, Regents of the University of Michigan
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package edu.umich.robot.soar;

import java.io.File;
import java.lang.Runtime;
import java.net.URLDecoder;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import sml.Agent;
import sml.smlAgentEventId;
import sml.Kernel.AgentEventInterface;
import april.config.Config;

import com.google.common.base.Splitter;
import com.google.common.collect.Lists;

import edu.umich.robot.RobotController;
import edu.umich.robot.RobotOutput;
import edu.umich.robot.events.control.AbstractControlEvent;
import edu.umich.robot.radio.RadioHandler;
import edu.umich.robot.radio.RadioMessage;
import edu.umich.robot.soar.AgentProperties.LearnSetting;
import edu.umich.robot.soar.AgentProperties.Mission;
import edu.umich.robot.util.AngSpeedUnit;
import edu.umich.robot.util.AngleResolution;
import edu.umich.robot.util.AngleUnit;
import edu.umich.robot.util.LengthUnit;
import edu.umich.robot.util.LinSpeedUnit;
import edu.umich.robot.util.Pose;
import edu.umich.robot.util.WallClock;
import edu.umich.robot.util.events.RobotEventListener;
import edu.umich.robot.util.events.RobotEventManager;
import edu.umich.robot.util.properties.BooleanPropertyProvider;
import edu.umich.robot.util.properties.DefaultPropertyProvider;
import edu.umich.robot.util.properties.EnumPropertyProvider;
import edu.umich.robot.util.properties.IntegerPropertyProvider;
import edu.umich.robot.util.properties.PropertyChangeEvent;
import edu.umich.robot.util.properties.PropertyKey;
import edu.umich.robot.util.properties.PropertyListener;
import edu.umich.robot.util.properties.PropertyManager;

/**
 * @author voigtjr@gmail.com
 */
public class SoarAgent implements RobotController, RadioHandler
{
	private static final Log logger = LogFactory.getLog(SoarAgent.class);

	private final PropertyManager properties = new PropertyManager();

	private final Agent agent;

	private final RobotOutput output;

	private final RobotEventManager events = new RobotEventManager();

	private InputLink il;

	private OutputLink ol;

	private final WaypointManager waypoints;

	private final ConcurrentMap<Long, RadioMessage> radioMessages = new ConcurrentHashMap<Long, RadioMessage>();

	private final EnumPropertyProvider<LengthUnit> lengthUnit = new EnumPropertyProvider<LengthUnit>(AgentProperties.LENGTH_UNIT);

	private final EnumPropertyProvider<LinSpeedUnit> linSpeedUnit = new EnumPropertyProvider<LinSpeedUnit>(AgentProperties.LINEAR_SPEED_UNIT);

	private final EnumPropertyProvider<AngSpeedUnit> angSpeedUnit = new EnumPropertyProvider<AngSpeedUnit>(AgentProperties.ANGULAR_SPEED_UNIT);

	private final EnumPropertyProvider<AngleUnit> angleUnit = new EnumPropertyProvider<AngleUnit>(AgentProperties.ANGLE_UNIT);

	private final EnumPropertyProvider<AngleResolution> angleResolution = new EnumPropertyProvider<AngleResolution>(AgentProperties.ANGLE_RESOLUTION);

	private final DefaultPropertyProvider<Double> linearVelocity = new DefaultPropertyProvider<Double>(AgentProperties.LINEAR_VELOCITY);

	private final DefaultPropertyProvider<Pose> translation = new DefaultPropertyProvider<Pose>(AgentProperties.TRANSLATION);

	private final IntegerPropertyProvider lingerSeconds = new IntegerPropertyProvider(AgentProperties.OBJECT_LINGER_SECONDS);

	private final EnumPropertyProvider<LearnSetting> learn = new EnumPropertyProvider<LearnSetting>(AgentProperties.LEARN);

	private final BooleanPropertyProvider epmemLearn = new BooleanPropertyProvider(AgentProperties.EPMEM_LEARNING);

	private final BooleanPropertyProvider smemLearn = new BooleanPropertyProvider(AgentProperties.SMEM_LEARNING);

	private final DefaultPropertyProvider<String[]> epmemExclusions = new DefaultPropertyProvider<String[]>(AgentProperties.EPMEM_EXCLUSIONS);

	private final DefaultPropertyProvider<String> defaultStorageAreaId = new DefaultPropertyProvider<String>(AgentProperties.DEFAULT_STORAGE_AREA_ID);

	private final DefaultPropertyProvider<String> areasHeldIn = new DefaultPropertyProvider<String>(AgentProperties.AREAS_HELD_IN);

	private final DefaultPropertyProvider<String> objectsHeldIn = new DefaultPropertyProvider<String>(AgentProperties.OBJECTS_HELD_IN);

	private final DefaultPropertyProvider<String> lookAheadPlanning = new DefaultPropertyProvider<String>(AgentProperties.LOOK_AHEAD_PLANNING);

	private final DefaultPropertyProvider<String> searchControlGoToGateway = new DefaultPropertyProvider<String>(AgentProperties.SEARCH_CONTROL_GO_TO_GATEWAY);

	private final DefaultPropertyProvider<String> deleteOldAreas = new DefaultPropertyProvider<String>(AgentProperties.DELETE_OLD_AREAS);

	private final DefaultPropertyProvider<Mission> mission = new DefaultPropertyProvider<Mission>(AgentProperties.MISSION);

	private final DefaultPropertyProvider<String[]> miscCommands = new DefaultPropertyProvider<String[]>(AgentProperties.MISC_COMMANDS);


	private final DefaultPropertyProvider<String> experimentName = new DefaultPropertyProvider<String>(DeliveryProperties.EXPERIMENT_NAME);
	private final DefaultPropertyProvider<Integer> trialNum = new DefaultPropertyProvider<Integer>(DeliveryProperties.TRIAL_NUM);
	private final DefaultPropertyProvider<String> logFile = new DefaultPropertyProvider<String>(DeliveryProperties.LOG_FILE);
	private final DefaultPropertyProvider<Integer> randSeed = new DefaultPropertyProvider<Integer>(DeliveryProperties.RAND_SEED);
	private final DefaultPropertyProvider<Integer> numBlocks = new DefaultPropertyProvider<Integer>(DeliveryProperties.NUM_BLOCKS);
	private final DefaultPropertyProvider<Integer> numTasks = new DefaultPropertyProvider<Integer>(DeliveryProperties.NUM_TASKS);
	private final DefaultPropertyProvider<String> tasksFile = new DefaultPropertyProvider<String>(DeliveryProperties.TASKS_FILE);
	private final DefaultPropertyProvider<Double> decayRate = new DefaultPropertyProvider<Double>(DeliveryProperties.DECAY_RATE);
	private final DefaultPropertyProvider<String> tasksHeldIn = new DefaultPropertyProvider<String>(DeliveryProperties.TASKS_HELD_IN);
	private final DefaultPropertyProvider<Integer> maxPatrolCircuits = new DefaultPropertyProvider<Integer>(DeliveryProperties.MAX_PATROL_CIRCUITS);
	private final DefaultPropertyProvider<Boolean> methodEcologicalObjects = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_OBJECTS);
	private final DefaultPropertyProvider<Boolean> methodEcologicalTiming = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_TIMING);
	private final DefaultPropertyProvider<Integer> methodEcologicalTimingInterval = new DefaultPropertyProvider<Integer>(DeliveryProperties.METHOD_ECOLOGICAL_TIMING_INTERVAL);
	private final DefaultPropertyProvider<Boolean> methodEcologicalDoors = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_DOORS);
	private final DefaultPropertyProvider<Boolean> methodEcologicalEntry = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_ENTRY);
	private final DefaultPropertyProvider<Boolean> methodEcologicalRehearsal = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL);
	private final DefaultPropertyProvider<Integer> methodEcologicalRehearsalAmount = new DefaultPropertyProvider<Integer>(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL_AMOUNT);
	private final DefaultPropertyProvider<Boolean> methodEcologicalRetrieval = new DefaultPropertyProvider<Boolean>(DeliveryProperties.METHOD_ECOLOGICAL_RETRIEVAL);

	private String productions;

	private final WallClock clock;

	private static final String SP_PARAM = 
		"sp {robot*elaborate*state*%s\n"
		+ "    (state <s> ^superstate nil\n"
		+ "               ^parameters <p>)\n"
		+ "-->\n"
		+ "    (<p> ^%s |%s|)\n"
		+ "}\n";

	public SoarAgent(Agent agent, RobotOutput output, WallClock clock, Config propc)
	{
		this.agent = agent;
		this.output = output;
		this.clock = clock;
		this.waypoints = new WaypointManager(getName());

		if (propc != null)
		{
			setDefaults(propc);
		}

		properties.setProvider(AgentProperties.LENGTH_UNIT, lengthUnit);
		properties.setProvider(AgentProperties.LINEAR_SPEED_UNIT, linSpeedUnit);
		properties.setProvider(AgentProperties.ANGULAR_SPEED_UNIT, angSpeedUnit);
		properties.setProvider(AgentProperties.ANGLE_UNIT, angleUnit);
		properties.setProvider(AgentProperties.ANGLE_RESOLUTION, angleResolution);
		properties.setProvider(AgentProperties.TRANSLATION, translation);
		properties.setProvider(AgentProperties.OBJECT_LINGER_SECONDS, lingerSeconds);

		properties.addListener(AgentProperties.LEARN, new PropertyListener<LearnSetting>() {
			public void propertyChanged(PropertyChangeEvent<LearnSetting> event) {
				SoarAgent.this.agent.ExecuteCommandLine(event.getNewValue().toCommandLine());
			}
		});
		properties.setProvider(AgentProperties.LEARN, learn);

		properties.addListener(AgentProperties.EPMEM_LEARNING, new PropertyListener<Boolean>() {
			public void propertyChanged(PropertyChangeEvent<Boolean> event) {
				if (event.getNewValue()) {
					SoarAgent.this.agent.ExecuteCommandLine("epmem -s learning on");
					SoarAgent.this.agent.ExecuteCommandLine("epmem -s timers three");
				} else {
					SoarAgent.this.agent.ExecuteCommandLine("epmem -s learning off");
				}
			}
		});
		properties.setProvider(AgentProperties.EPMEM_LEARNING, epmemLearn);

		properties.addListener(AgentProperties.SMEM_LEARNING, new PropertyListener<Boolean>() {
			public void propertyChanged(PropertyChangeEvent<Boolean> event) {
				if (event.getNewValue()) {
					SoarAgent.this.agent.ExecuteCommandLine("smem -s learning on");
					SoarAgent.this.agent.ExecuteCommandLine("smem -s timers three");
				} else {
					SoarAgent.this.agent.ExecuteCommandLine("smem -s learning off");
				}
			}
		});
		properties.setProvider(AgentProperties.SMEM_LEARNING, smemLearn);

		// because epmem exclusions are toggled, we need to turn on all the defaults 
		// except the two that are on by default (epmem, smem)
		for (String s : epmemExclusions.get()) {
			if (s.equals("epmem") || s.equals("smem"))
				continue;
			agent.ExecuteCommandLine("epmem -s exclusions " + s);
		}

		// This whole step verifies the setting by asking the agent. It isn't necessary but is here for a sanity check.
		// BUGBUG: agents can change this behind our back (in firstload). It needs to be updated before use.
		Splitter splitter = Splitter.on(", ").trimResults();
		List<String> exclList = Lists.newArrayList(splitter.split(agent.ExecuteCommandLine("epmem -g exclusions")));
		epmemExclusions.set(exclList.toArray(new String[exclList.size()]));
		logger.debug("Initial epmem exclusions: " + Arrays.toString(epmemExclusions.get()));

		// Setting provider before adding listener so that event doesn't fire.
		properties.setProvider(AgentProperties.EPMEM_EXCLUSIONS, epmemExclusions);
		properties.addListener(AgentProperties.EPMEM_EXCLUSIONS, new PropertyListener<String[]>() {
			public void propertyChanged(PropertyChangeEvent<String[]> event) {
				// removes old values
				for (String s : event.getOldValue()) {
					logger.debug("epmem -s exclusions " + s);
					SoarAgent.this.agent.ExecuteCommandLine("epmem -s exclusions " + s);
				}

				for (String s : event.getNewValue()) {
					logger.debug("epmem -s exclusions " + s);
					SoarAgent.this.agent.ExecuteCommandLine("epmem -s exclusions " + s);
				}

				logger.debug("exclusions after update: " + SoarAgent.this.agent.ExecuteCommandLine("epmem -g exclusions").trim());
			}
		});

		addSpProperty(AgentProperties.LINEAR_VELOCITY, linearVelocity);

		addSpProperty(AgentProperties.DEFAULT_STORAGE_AREA_ID, defaultStorageAreaId);
		addSpProperty(AgentProperties.AREAS_HELD_IN, areasHeldIn);
		addSpProperty(AgentProperties.OBJECTS_HELD_IN, objectsHeldIn);
		addSpProperty(AgentProperties.LOOK_AHEAD_PLANNING, lookAheadPlanning);
		addSpProperty(AgentProperties.SEARCH_CONTROL_GO_TO_GATEWAY, searchControlGoToGateway);
		addSpProperty(AgentProperties.DELETE_OLD_AREAS, deleteOldAreas);

		addSpProperty(DeliveryProperties.EXPERIMENT_NAME, experimentName);
		addSpProperty(DeliveryProperties.TRIAL_NUM, trialNum);
		addSpProperty(DeliveryProperties.LOG_FILE, logFile);
		addSpProperty(DeliveryProperties.RAND_SEED, randSeed);
		addSpProperty(DeliveryProperties.NUM_BLOCKS, numBlocks);
		addSpProperty(DeliveryProperties.NUM_TASKS, numTasks);
		addSpProperty(DeliveryProperties.TASKS_FILE, tasksFile);
		addSpProperty(DeliveryProperties.TASKS_HELD_IN, tasksHeldIn);
		addSpProperty(DeliveryProperties.MAX_PATROL_CIRCUITS, maxPatrolCircuits);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_OBJECTS, methodEcologicalObjects);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_TIMING, methodEcologicalTiming);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_TIMING_INTERVAL, methodEcologicalTimingInterval);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_DOORS, methodEcologicalDoors);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_ENTRY, methodEcologicalEntry);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL, methodEcologicalRehearsal);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL_AMOUNT, methodEcologicalRehearsalAmount);
		addSpProperty(DeliveryProperties.METHOD_ECOLOGICAL_RETRIEVAL, methodEcologicalRetrieval);
		properties.addListener(DeliveryProperties.DECAY_RATE, new PropertyListener<Double>() {
			public void propertyChanged(PropertyChangeEvent<Double> event) {
				SoarAgent.this.agent.ExecuteCommandLine("wma -s activation off");
				SoarAgent.this.agent.ExecuteCommandLine("wma -s forgetting off");
				SoarAgent.this.agent.ExecuteCommandLine("wma -s decay-rate " + event.getNewValue());
				SoarAgent.this.agent.ExecuteCommandLine("wma -s forgetting on");
				SoarAgent.this.agent.ExecuteCommandLine("wma -s forget-wme lti");
				SoarAgent.this.agent.ExecuteCommandLine("wma -s activation on");
			}
		});
		// this must be after the additional listener above
		addSpProperty(DeliveryProperties.DECAY_RATE, decayRate);

		addSpProperty(AgentProperties.MISSION, mission);
		properties.addListener(AgentProperties.MISC_COMMANDS, new PropertyListener<String[]>() {
			public void propertyChanged(PropertyChangeEvent<String[]> event) {
				for (String s : event.getNewValue()) {
					logger.debug(s);
					SoarAgent.this.agent.ExecuteCommandLine(s);
				}
			}
		});
		properties.setProvider(AgentProperties.MISC_COMMANDS, miscCommands);
	}

	private void setDefaults(Config propc)
	{
		if (propc.hasKey(AgentProperties.LINEAR_VELOCITY.getName()))
			linearVelocity.set(Double.valueOf(propc.requireString(AgentProperties.LINEAR_VELOCITY.getName())));

		if (propc.hasKey(AgentProperties.LEARN.getName()))
			learn.set(LearnSetting.valueOf(propc.requireString(AgentProperties.LEARN.getName())));
		if (propc.hasKey(AgentProperties.EPMEM_LEARNING.getName()))
			epmemLearn.set(Boolean.valueOf(propc.requireString(AgentProperties.EPMEM_LEARNING.getName())));
		if (propc.hasKey(AgentProperties.SMEM_LEARNING.getName()))
			smemLearn.set(Boolean.valueOf(propc.requireString(AgentProperties.SMEM_LEARNING.getName())));
		if (propc.hasKey(AgentProperties.EPMEM_EXCLUSIONS.getName()))
			epmemExclusions.set(propc.requireStrings(AgentProperties.EPMEM_EXCLUSIONS.getName()));
		if (propc.hasKey(AgentProperties.DEFAULT_STORAGE_AREA_ID.getName()))
			defaultStorageAreaId.set(propc.requireString(AgentProperties.DEFAULT_STORAGE_AREA_ID.getName()));
		if (propc.hasKey(AgentProperties.AREAS_HELD_IN.getName()))
			areasHeldIn.set(propc.requireString(AgentProperties.AREAS_HELD_IN.getName()));
		if (propc.hasKey(AgentProperties.OBJECTS_HELD_IN.getName()))
			objectsHeldIn.set(propc.requireString(AgentProperties.OBJECTS_HELD_IN.getName()));
		if (propc.hasKey(AgentProperties.LOOK_AHEAD_PLANNING.getName()))
			lookAheadPlanning.set(propc.requireString(AgentProperties.LOOK_AHEAD_PLANNING.getName()));
		if (propc.hasKey(AgentProperties.SEARCH_CONTROL_GO_TO_GATEWAY.getName()))
			searchControlGoToGateway.set(propc.requireString(AgentProperties.SEARCH_CONTROL_GO_TO_GATEWAY.getName()));
		if (propc.hasKey(AgentProperties.DELETE_OLD_AREAS.getName()))
			deleteOldAreas.set(propc.requireString(AgentProperties.DELETE_OLD_AREAS.getName()));
		if (propc.hasKey(AgentProperties.MISSION.getName()))
			mission.set(Mission.valueOf(propc.requireString(AgentProperties.MISSION.getName())));
		if (propc.hasKey(AgentProperties.MISC_COMMANDS.getName()))
			miscCommands.set(propc.requireStrings(AgentProperties.MISC_COMMANDS.getName()));

		if (propc.hasKey(DeliveryProperties.EXPERIMENT_NAME.getName()))
			experimentName.set(propc.requireString(DeliveryProperties.EXPERIMENT_NAME.getName()));
		if (propc.hasKey(DeliveryProperties.TRIAL_NUM.getName()))
			trialNum.set(Integer.valueOf(propc.requireString(DeliveryProperties.TRIAL_NUM.getName())));
		if (propc.hasKey(DeliveryProperties.LOG_FILE.getName()))
			logFile.set(propc.requireString(DeliveryProperties.LOG_FILE.getName()));
		if (propc.hasKey(DeliveryProperties.RAND_SEED.getName()))
			randSeed.set(Integer.valueOf(propc.requireString(DeliveryProperties.RAND_SEED.getName())));
		if (propc.hasKey(DeliveryProperties.TRIAL_NUM.getName()))
			numBlocks.set(Integer.valueOf(propc.requireString(DeliveryProperties.NUM_BLOCKS.getName())));
		if (propc.hasKey(DeliveryProperties.TRIAL_NUM.getName()))
			numTasks.set(Integer.valueOf(propc.requireString(DeliveryProperties.NUM_TASKS.getName())));
		if (propc.hasKey(DeliveryProperties.TASKS_FILE.getName()))
			tasksFile.set(propc.requireString(DeliveryProperties.TASKS_FILE.getName()));
		if (propc.hasKey(DeliveryProperties.DECAY_RATE.getName()))
			decayRate.set(Double.valueOf(propc.requireString(DeliveryProperties.DECAY_RATE.getName())));
		if (propc.hasKey(DeliveryProperties.TASKS_HELD_IN.getName()))
			tasksHeldIn.set(propc.requireString(DeliveryProperties.TASKS_HELD_IN.getName()));
		if (propc.hasKey(DeliveryProperties.MAX_PATROL_CIRCUITS.getName()))
			maxPatrolCircuits.set(Integer.valueOf(propc.requireString(DeliveryProperties.MAX_PATROL_CIRCUITS.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_OBJECTS.getName()))
			methodEcologicalObjects.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_OBJECTS.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_TIMING.getName()))
			methodEcologicalTiming.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_TIMING.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_TIMING_INTERVAL.getName()))
			methodEcologicalTimingInterval.set(Integer.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_TIMING_INTERVAL.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_DOORS.getName()))
			methodEcologicalDoors.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_DOORS.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_ENTRY.getName()))
			methodEcologicalEntry.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_ENTRY.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL.getName()))
			methodEcologicalRehearsal.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL_AMOUNT.getName()))
			methodEcologicalRehearsalAmount.set(Integer.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_REHEARSAL_AMOUNT.getName())));
		if (propc.hasKey(DeliveryProperties.METHOD_ECOLOGICAL_RETRIEVAL.getName()))
			methodEcologicalRetrieval.set(Boolean.valueOf(propc.requireString(DeliveryProperties.METHOD_ECOLOGICAL_RETRIEVAL.getName())));
	}

	private <T> void addSpProperty(PropertyKey<T> key, DefaultPropertyProvider<T> prov)
	{
		properties.addListener(key, new PropertyListener<T>() {
			public void propertyChanged(PropertyChangeEvent<T> event) {
				SoarAgent.this.agent.ExecuteCommandLine(makeSpParam(event.getKey().toString(), event.getNewValue()));
			}
		});
		properties.setProvider(key, prov);
	}


	private <T> String makeSpParam(String key, T value)
	{
		return String.format(SP_PARAM, key, key, value.toString());
	}

	public void initialize() 
	{
		agent.SetBlinkIfNoChange(false);

		il = InputLink.newInstance(this);

		agent.Commit();
	}

	public PropertyManager getProperties()
	{
		return properties;
	}

	Agent getSoarAgent()
	{   
		return agent;
	}

	RobotEventManager getEvents()
	{
		return events;
	}

	WaypointManager getWaypoints()
	{
		return waypoints;
	}

	ConcurrentMap<Long, RadioMessage> getRadioMessages()
	{
		return radioMessages;
	}

	private void destroy()
	{
		if (il != null)
			il.destroy();
		il = null;

		if (ol != null)
			ol.destroy();
		ol = null;
	}

	public void loadProductions(String productions) throws SoarException
	{
		agent.ExecuteCommandLine("waitsnc -d");
		if (!agent.LoadProductions(productions))
		{
			String message = agent.GetLastErrorDescription();
			agent.ExecuteCommandLine("waitsnc -e");
			throw new SoarException(message);
		}
		this.productions = productions;
	}

	public String getProductionsFile()
	{
		return productions;
	}

	public void seed(int seed)
	{
		logger.warn(String.format("Setting random seed: %d", seed));
		agent.ExecuteCommandLine(String.format("srand %d", seed));
	}

	public void update()
	{
		if (ol == null)
			if (agent.GetOutputLink() != null)
				ol = OutputLink.newInstance(this);

		if (ol != null)
			ol.update();
		il.update();
		logger.trace("IO Update done");
		agent.Commit();
	}

	public void debug()
	{
		try {
			SoarProperties sp = new SoarProperties();
			// sp.spawnDebugger(agent);
			String jarPath = SoarAgent.class.getProtectionDomain().getCodeSource().getLocation().getPath();
			jarPath = URLDecoder.decode(jarPath, "utf-8");
			jarPath = new File(jarPath).getPath();
			boolean isOSX = System.getProperty("os.name").equals("Mac OS X");
			int port = agent.GetKernel().GetListenerPort();
			String command = "java " + (isOSX ? "-XstartOnFirstThread " : "") + "-jar " + jarPath + " -debugger -remote -port " + port + " -agent " + getName();
			System.out.println("Spawning debugger with command: \"" + command + "\"");
			Runtime.getRuntime().exec(command);
			System.out.println("Done spawning debugger");
		} catch (java.io.IOException e) {
			e.printStackTrace();
		}
	}

	public <T extends AbstractControlEvent> void addListener(Class<T> klass,
			RobotEventListener listener)
	{
		events.addListener(klass, listener);
	}

	public String getName()
	{
		return agent.GetAgentName();
	}

	public <T extends AbstractControlEvent> void removeListener(Class<T> klass,
			RobotEventListener listener)
	{
		events.removeListener(klass, listener);
	}

	private final AgentEventInterface agentHandler = new AgentEventInterface()
	{
		public void agentEventHandler(int eventID, Object data, String agentName)
		{
			if (eventID == smlAgentEventId.smlEVENT_BEFORE_AGENT_REINITIALIZED
					.swigValue())
			{
				logger.trace("smlEVENT_BEFORE_AGENT_REINITIALIZED");

				destroy();

				agent.Commit();
				logger.trace("smlEVENT_BEFORE_AGENT_REINITIALIZED done");

			}
			else if (eventID == smlAgentEventId.smlEVENT_AFTER_AGENT_REINITIALIZED
					.swigValue())
			{
				logger.trace("smlEVENT_AFTER_AGENT_REINITIALIZED");

				il = InputLink.newInstance(SoarAgent.this);

				agent.Commit();
				logger.trace("smlEVENT_AFTER_AGENT_REINITIALIZED done");
			}
		}
	};

	public AgentEventInterface getAgentHandler()
	{
		return agentHandler;
	}

	@Override
		public String toString()
		{
			return "Soar: " + getName();
		}

	public RobotOutput getRobotOutput()
	{
		return output;
	}

	void stopEvent()
	{
		if (ol != null)
			ol.stopEvent();
	}

	void startEvent()
	{
		if (ol != null)
			ol.startEvent();
	}

	public WallClock getWallClock()
	{
		return clock;
	}

	public void shutdown()
	{
		waypoints.shutdown();
	}

	@Override
		public void radioMessageReceived(RadioMessage comm) {
			radioMessages.put((long) comm.getId(), comm);
		}

}
