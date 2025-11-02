#!/usr/bin/env tsx

/**
 * Event Seeding Script for Daily Reminder Calendar
 *
 * This script populates the Qt backend with realistic sample events
 * for testing the calendar module.
 *
 * Usage:
 *   npm run seed
 *
 * Or directly:
 *   npx tsx scripts/seed-events.ts
 */

interface IEvent {
  id?: string;
  category: string;
  startDate: string;
  endDate: string;
  title: string;
  color: "blue" | "green" | "red" | "yellow" | "purple" | "orange";
  description: string;
  reminderTime?: string;
  isReminderEnabled?: boolean;
}

const API_BASE_URL = "http://localhost:8080";

const COLORS: IEvent["color"][] = [
  "blue",
  "green",
  "red",
  "yellow",
  "purple",
  "orange",
];

const CATEGORIES = [
  "Work",
  "Personal",
  "Health",
  "Shopping",
  "Exercise",
  "Study",
  "Social",
  "Home",
];

/**
 * Helper to format date as ISO string (YYYY-MM-DDTHH:mm:ss.sssZ)
 */
function toISOString(date: Date): string {
  return date.toISOString();
}

/**
 * Generate a date relative to today
 */
function addDays(date: Date, days: number): Date {
  const result = new Date(date);
  result.setDate(result.getDate() + days);
  return result;
}

/**
 * Create an event via Qt backend API
 */
async function createEvent(event: Omit<IEvent, "id">): Promise<IEvent> {
  const response = await fetch(`${API_BASE_URL}/api/event`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(event),
  });

  if (!response.ok) {
    const error = await response.text();
    throw new Error(`Failed to create event: ${response.status} - ${error}`);
  }

  return response.json();
}

/**
 * Check if backend is accessible
 */
async function checkBackend(): Promise<boolean> {
  try {
    const response = await fetch(`${API_BASE_URL}/status`);
    return response.ok;
  } catch (error) {
    return false;
  }
}

/**
 * Generate sample events - Personal daily activities
 */
function generateSampleEvents(): Omit<IEvent, "id">[] {
  const today = new Date();
  today.setHours(0, 0, 0, 0);

  // Helper to set time
  const setTime = (date: Date, hours: number, minutes: number = 0) => {
    const d = new Date(date);
    d.setHours(hours, minutes, 0, 0);
    return d;
  };

  // Helper to create reminder (30 min before)
  const createReminder = (eventDate: Date) => {
    const reminder = new Date(eventDate);
    reminder.setMinutes(reminder.getMinutes() - 30);
    return toISOString(reminder);
  };

  return [
    // Past events
    {
      category: "Health",
      startDate: toISOString(setTime(addDays(today, -7), 8)),
      endDate: toISOString(setTime(addDays(today, -7), 9)),
      title: "Morning Jog",
      color: "green",
      description: "30-minute jog at the park",
      isReminderEnabled: false,
    },
    {
      category: "Work",
      startDate: toISOString(setTime(addDays(today, -5), 9)),
      endDate: toISOString(setTime(addDays(today, -5), 17)),
      title: "Client Meeting",
      color: "blue",
      description: "Quarterly review with ABC Corp",
      isReminderEnabled: false,
    },

    // Today's events
    {
      category: "Work",
      startDate: toISOString(setTime(today, 10)),
      endDate: toISOString(setTime(today, 11, 30)),
      title: "Team Standup",
      color: "blue",
      description: "Daily sync with development team",
      reminderTime: createReminder(setTime(today, 10)),
      isReminderEnabled: true,
    },
    {
      category: "Personal",
      startDate: toISOString(setTime(today, 14)),
      endDate: toISOString(setTime(today, 15)),
      title: "Dentist Appointment",
      color: "red",
      description: "Regular checkup at Smile Dental Clinic",
      reminderTime: createReminder(setTime(today, 14)),
      isReminderEnabled: true,
    },
    {
      category: "Shopping",
      startDate: toISOString(setTime(today, 18)),
      endDate: toISOString(setTime(today, 19)),
      title: "Grocery Shopping",
      color: "yellow",
      description: "Weekly groceries at supermarket",
      reminderTime: createReminder(setTime(today, 18)),
      isReminderEnabled: true,
    },

    // Tomorrow
    {
      category: "Exercise",
      startDate: toISOString(setTime(addDays(today, 1), 6)),
      endDate: toISOString(setTime(addDays(today, 1), 7)),
      title: "Gym Workout",
      color: "orange",
      description: "Leg day - squats, lunges, leg press",
      reminderTime: createReminder(setTime(addDays(today, 1), 6)),
      isReminderEnabled: true,
    },
    {
      category: "Study",
      startDate: toISOString(setTime(addDays(today, 1), 19)),
      endDate: toISOString(setTime(addDays(today, 1), 21)),
      title: "Online Course: React Advanced",
      color: "purple",
      description: "Complete modules 3-5 on Udemy",
      reminderTime: createReminder(setTime(addDays(today, 1), 19)),
      isReminderEnabled: true,
    },

    // This week
    {
      category: "Social",
      startDate: toISOString(setTime(addDays(today, 3), 19)),
      endDate: toISOString(setTime(addDays(today, 3), 22)),
      title: "Dinner with Friends",
      color: "purple",
      description: "Italian restaurant downtown",
      reminderTime: createReminder(setTime(addDays(today, 3), 19)),
      isReminderEnabled: true,
    },
    {
      category: "Home",
      startDate: toISOString(setTime(addDays(today, 4), 10)),
      endDate: toISOString(setTime(addDays(today, 4), 12)),
      title: "House Cleaning",
      color: "green",
      description: "Deep clean living room and kitchen",
      reminderTime: createReminder(setTime(addDays(today, 4), 10)),
      isReminderEnabled: true,
    },
    {
      category: "Personal",
      startDate: toISOString(setTime(addDays(today, 5), 15)),
      endDate: toISOString(setTime(addDays(today, 5), 16)),
      title: "Haircut",
      color: "yellow",
      description: "Barber appointment at 3 PM",
      reminderTime: createReminder(setTime(addDays(today, 5), 15)),
      isReminderEnabled: true,
    },

    // Next week
    {
      category: "Work",
      startDate: toISOString(setTime(addDays(today, 7), 9)),
      endDate: toISOString(setTime(addDays(today, 7), 12)),
      title: "Project Presentation",
      color: "red",
      description: "Q4 roadmap presentation to stakeholders",
      reminderTime: createReminder(setTime(addDays(today, 7), 9)),
      isReminderEnabled: true,
    },
    {
      category: "Health",
      startDate: toISOString(setTime(addDays(today, 10), 8)),
      endDate: toISOString(setTime(addDays(today, 10), 9)),
      title: "Yoga Class",
      color: "green",
      description: "Hatha yoga at community center",
      reminderTime: createReminder(setTime(addDays(today, 10), 8)),
      isReminderEnabled: true,
    },

    // Multi-day events
    {
      category: "Personal",
      startDate: toISOString(setTime(addDays(today, 14), 0)),
      endDate: toISOString(setTime(addDays(today, 16), 23, 59)),
      title: "Weekend Trip",
      color: "orange",
      description: "Beach vacation with family",
      reminderTime: createReminder(setTime(addDays(today, 14), 6)),
      isReminderEnabled: true,
    },
    {
      category: "Study",
      startDate: toISOString(setTime(addDays(today, 20), 18)),
      endDate: toISOString(setTime(addDays(today, 20), 20)),
      title: "Prepare for Certification Exam",
      color: "blue",
      description: "AWS Solutions Architect study session",
      reminderTime: createReminder(setTime(addDays(today, 20), 18)),
      isReminderEnabled: true,
    },
    {
      category: "Social",
      startDate: toISOString(setTime(addDays(today, 25), 14)),
      endDate: toISOString(setTime(addDays(today, 25), 18)),
      title: "Birthday Party",
      color: "purple",
      description: "Friend's birthday celebration",
      reminderTime: createReminder(setTime(addDays(today, 25), 14)),
      isReminderEnabled: true,
    },
  ];
}

/**
 * Main seeding function
 */
async function seedEvents() {
  console.log("🌱 Daily Reminder - Event Seeding Script\n");
  console.log(`📡 Checking backend at ${API_BASE_URL}...`);

  // Check backend availability
  const isBackendUp = await checkBackend();
  if (!isBackendUp) {
    console.error("❌ ERROR: Backend is not accessible!");
    console.error(
      `   Make sure the Qt application is running on ${API_BASE_URL}`
    );
    console.error("   Run: npm run dev");
    process.exit(1);
  }

  console.log("✅ Backend is online\n");

  const events = generateSampleEvents();
  console.log(`📝 Generating ${events.length} sample events...\n`);

  let successCount = 0;
  let failCount = 0;

  for (let i = 0; i < events.length; i++) {
    const event = events[i];
    try {
      const created = await createEvent(event);
      successCount++;
      console.log(`✅ [${i + 1}/${events.length}] Created: "${event.title}"`);
      console.log(
        `   └─ ID: ${created.id}, Category: ${event.category}, Color: ${event.color}`
      );
    } catch (error) {
      failCount++;
      console.error(`❌ [${i + 1}/${events.length}] Failed: "${event.title}"`);
      console.error(
        `   └─ Error: ${error instanceof Error ? error.message : String(error)}`
      );
    }
  }

  console.log("\n" + "=".repeat(60));
  console.log(`📊 Seeding Summary:`);
  console.log(`   ✅ Success: ${successCount}/${events.length}`);
  console.log(`   ❌ Failed:  ${failCount}/${events.length}`);
  console.log("=".repeat(60));

  if (failCount > 0) {
    console.log("\n⚠️  Some events failed to seed. Check the errors above.");
    process.exit(1);
  } else {
    console.log("\n🎉 All events seeded successfully!");
    console.log("   Open the calendar to see your events!");
  }
}

// Run the seeding script
seedEvents().catch((error) => {
  console.error("\n💥 Fatal error during seeding:");
  console.error(error);
  process.exit(1);
});
